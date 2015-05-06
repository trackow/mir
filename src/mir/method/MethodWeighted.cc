/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/method/MethodWeighted.h"

#include <cmath>
#include <limits>
#include <map>
#include <string>

#include "atlas/Grid.h"

#include "eckit/log/BigNum.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/data/MIRField.h"
#include "mir/lsm/LandSeaMask.h"
#include "mir/method/WeightCache.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"


static eckit::Mutex local_mutex;


namespace mir {
namespace method {


static std::map<std::string, MethodWeighted::Matrix> matrix_cache;


MethodWeighted::MethodWeighted(const param::MIRParametrisation& param) :
    Method(param) {
}


MethodWeighted::~MethodWeighted() {
}


// This returns a 'const' matrix so we ensure that we don't change it and break the cache
const MethodWeighted::Matrix& MethodWeighted::getMatrix(const atlas::Grid& in, const atlas::Grid& out) const {

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    std::string whash = hash(in, out);
    std::auto_ptr<lsm::LandSeaMask> mask_in(0);
    std::auto_ptr<lsm::LandSeaMask> mask_out(0);

    bool use_lsm = false;
    if (parametrisation_.get("use.lsm", use_lsm) && use_lsm) {
        mask_in.reset(lsm::LandSeaMaskFactory::build(parametrisation_));
        mask_out.reset(lsm::LandSeaMaskFactory::build(parametrisation_));
    }

    if (mask_in.get()) {
        whash += std::string(".IM") + mask_in->unique_id(in);
    }

    if (mask_out.get()) {
        whash += std::string(".OM") + mask_out->unique_id(out);
    }

    std::map<std::string, MethodWeighted::Matrix>::iterator j = matrix_cache.find(whash);
    if (j != matrix_cache.end()) {
        return (*j).second;
    }


    WeightCache cache;

// calculate weights matrix, apply mask if necessary

    MethodWeighted::Matrix W(out.npts(), in.npts());

    if (!cache.get(whash, W)) {
        if (in.unique_id() == out.unique_id() && in.same(out)) {
            W.setIdentity();
        } else {
            eckit::Timer t("Calculating interpolation weights");
            assemble(W, in, out);
        }

        // TODO: Apply LSMs

        if (mask_in.get() && mask_out.get()) {
            applyBothMask(W, in, out, *mask_in, *mask_out);
        } else if (mask_in.get()) {
            applyInputMask(W, in, out, *mask_in);
        } else if (mask_out.get()) {
            applyOutputMask(W, in, out, *mask_out);
        }

        // Mask should be considered in caching
        // applyMask(W, in, out);

        cache.add(whash, W);
    }

    std::swap(matrix_cache[whash], W);
    return matrix_cache[whash];
}


void MethodWeighted::execute(data::MIRField& field, const atlas::Grid& in, const atlas::Grid& out) const {
    eckit::Log::info() << "MethodWeighted::execute" << std::endl;

    size_t npts_inp = in.npts();
    size_t npts_out = out.npts();
    const MethodWeighted::Matrix& W = getMatrix(in, out);

    // TODO: ASSERT matrix size is npts_inp * npts_out


    // multiply interpolant matrix with field vector
    for (size_t i = 0; i < field.dimensions(); i++) {
        eckit::StrStream os;
        os << "Interpolating field ("  << npts_inp << " -> " << npts_out << ")" << eckit::StrStream::ends;
        std::string msg(os);
        eckit::Timer t(msg);

        ASSERT(field.values(i).size() == npts_inp);
        eckit::Log::info() << "Input field is " << field.values(i).size() << std::endl;

        std::vector<double>& values = field.values(i);
        std::vector<double> result(npts_out);

        Eigen::VectorXd::MapType vi = Eigen::VectorXd::Map( &values[0], npts_inp );
        Eigen::VectorXd::MapType vo = Eigen::VectorXd::Map( &result[0], npts_out );

        if (field.hasMissing()) {
            // Assumes compiler does return value optimization
            // otherwise we need to pass result matrix as parameter
            Matrix MW = applyMissingValues(W, field, i);
            vo = MW * vi;
        } else {
            vo = W * vi;
        }

        field.values(result, i); // Update field with result
    }
}


MethodWeighted::Matrix MethodWeighted::applyMissingValues(const MethodWeighted::Matrix& W, data::MIRField& field, size_t which) const {
    ASSERT(field.hasMissing());

    eckit::Log::info() << "Field has missing values" << std::endl;
    double missing = field.missingValue();
    const std::vector<double>& values = field.values(which);

    // setup sizes & counters
    const size_t
            Nivalues = values.size(),
            Novalues = W.rows();
    size_t
            count = 0,
            count_all_missing  = 0,
            count_some_missing = 0;

    std::vector< bool > missvalues(Nivalues);
    for (size_t i = 0; i < Nivalues; i++) {
        missvalues[i] = (values[i] == missing);
        if (values[i] == missing) {
            count++;
        }
    }

    if (count == 0) {
        return W;
    }
    eckit::Log::info() << "Field has " << eckit::Plural(count, "missing value") << " out of " << eckit::BigNum(Nivalues) << std::endl;


    // sparse matrix weigths redistribution
    MethodWeighted::Matrix X(W);
    for (size_t i = 0; i < X.rows(); i++) {

        // count missing values and accumulate weights
        double sum = 0.;
        size_t
                Nmiss = 0,
                Ncol  = 0;
        for (Matrix::InnerIterator j(X,i); j; ++j, ++Ncol) {
            if (missvalues[j.col()])
              ++Nmiss;
            else
              sum += j.value();
        }

        // redistribution
        if (!Nmiss) {

            // no missing values, no redistribution

        }
        else if ( (std::abs(sum)<std::numeric_limits< double >::epsilon()) ||
                  (Ncol==Nmiss) ) {
            ++count_all_missing;

            // all values are missing (or weights wrongly computed), special case
            // (it covers Ncol>0 with the above condition)
            for (Matrix::InnerIterator j(X,i); j; ++j) {
                j.valueRef() = 0.;
                field.values(which)[j.col()] = missing;
            }
            Matrix::InnerIterator(X,i).valueRef() = 1.;

        }
        else {
            ++count_some_missing;

            // apply linear redistribution
            // TODO: new redistribution methods
            const double invsum = 1/sum;
            for (Matrix::InnerIterator j(X,i); j; ++j) {
                if (missvalues[j.col()]) {
                  field.values(which)[j.col()] = missing;
                  j.valueRef() = 0.;
                }
                else {
                  j.valueRef() *= invsum;
                }
            }

        }
    }

    // log corrections and return
    eckit::Log::info() << "Missing value corrections: " << count_some_missing << '/' << count_all_missing << " some/all-missing out of " << eckit::BigNum(Novalues) << std::endl;
    return X;
}


void MethodWeighted::applyInputMask(Matrix& W, const atlas::Grid& in, const atlas::Grid& out, const lsm::LandSeaMask& imask) const {

    return; // For now

    std::auto_ptr<data::MIRField> imask_field(imask.field(in));
}


void MethodWeighted::applyOutputMask(Matrix& W, const atlas::Grid& in, const atlas::Grid& out, const lsm::LandSeaMask& omask) const {

    return; // For now

    std::auto_ptr<data::MIRField> omask_field(omask.field(out));
}


void MethodWeighted::applyBothMask(Matrix& W, const atlas::Grid& in, const atlas::Grid& out,
    const lsm::LandSeaMask& imask, const lsm::LandSeaMask& omask) const {

    return; // For now

    std::auto_ptr<data::MIRField> imask_field(imask.field(in));
    std::auto_ptr<data::MIRField> omask_field(omask.field(out));
}


std::string MethodWeighted::hash(const atlas::Grid& in, const atlas::Grid& out) const {
    std::ostringstream os;
    os << name() << "." << in.unique_id() << "." << out.unique_id();
    return os.str();
}


}  // namespace method
}  // namespace mir

