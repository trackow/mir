/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/nonlinear/MissingIfHeaviestMissing.h"

#include <ostream>
#include <sstream>

#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/util/Exceptions.h"


namespace mir {
namespace method {
namespace nonlinear {


MissingIfHeaviestMissing::MissingIfHeaviestMissing(const param::MIRParametrisation& param) : NonLinear(param) {}


bool MissingIfHeaviestMissing::treatment(MethodWeighted::Matrix&, MethodWeighted::WeightMatrix& W,
                                         MethodWeighted::Matrix&, const MIRValuesVector& values,
                                         const double& missingValue) const {

    // correct matrix weigths for the missing values
    ASSERT(W.cols() == values.size());

    auto data  = const_cast<WeightMatrix::Scalar*>(W.data());
    bool modif = false;

    WeightMatrix::Size i = 0;
    WeightMatrix::iterator it(W);
    for (WeightMatrix::Size r = 0; r < W.rows(); ++r) {
        const WeightMatrix::iterator end = W.end(r);

        // count missing values, accumulate weights (disregarding missing values) and find maximum weight in row
        size_t i_missing         = i;
        size_t N_missing         = 0;
        size_t N_entries         = 0;
        double sum               = 0.;
        double heaviest          = -1.;
        bool heaviest_is_missing = false;

        WeightMatrix::iterator kt(it);
        WeightMatrix::Size k = i;
        for (; it != end; ++it, ++i, ++N_entries) {

            const bool miss = values[it.col()] == missingValue;

            if (miss) {
                ++N_missing;
                i_missing = i;
            }
            else {
                sum += *it;
            }

            if (heaviest < data[i]) {
                heaviest            = data[i];
                heaviest_is_missing = miss;
            }
        }

        // weights redistribution: zero-weight all missing values, linear re-weighting for the others;
        // if all values are missing, or the closest value is missing, force missing value
        if (N_missing > 0) {
            if (N_missing == N_entries || heaviest_is_missing || eckit::types::is_approximately_equal(sum, 0.)) {

                for (WeightMatrix::Size j = k; j < k + N_entries; ++j) {
                    data[j] = j == i_missing ? 1. : 0.;
                }
            }
            else {

                const double factor = 1. / sum;
                for (WeightMatrix::Size j = k; j < k + N_entries; ++j, ++kt) {
                    const bool miss = values[kt.col()] == missingValue;
                    data[j]         = miss ? 0. : (factor * data[j]);
                }
            }
            modif = true;
        }
    }

    return modif;
}


bool MissingIfHeaviestMissing::sameAs(const NonLinear& other) const {
    auto o = dynamic_cast<const MissingIfHeaviestMissing*>(&other);
    return (o != nullptr);
}


void MissingIfHeaviestMissing::print(std::ostream& out) const {
    out << "MissingIfHeaviestMissing[]";
}


void MissingIfHeaviestMissing::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static NonLinearBuilder<MissingIfHeaviestMissing> __nonlinear("missing-if-heaviest-missing");


}  // namespace nonlinear
}  // namespace method
}  // namespace mir
