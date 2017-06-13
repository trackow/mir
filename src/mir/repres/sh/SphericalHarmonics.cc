/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/repres/sh/SphericalHarmonics.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace sh {


SphericalHarmonics::SphericalHarmonics(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("truncation", truncation_));
    ASSERT(parametrisation.get("TS", Ts_));
}


SphericalHarmonics::SphericalHarmonics(size_t truncation) :
    truncation_(truncation) {
}


SphericalHarmonics::~SphericalHarmonics() {
}


void SphericalHarmonics::print(std::ostream &out) const {
    out << "SphericalHarmonics["
        << "truncation=" << truncation_
        << "]";
}


void SphericalHarmonics::makeName(std::ostream& out) const {
    out << "T" << truncation_;
}


bool SphericalHarmonics::sameAs(const Representation& other) const { NOTIMP; }


bool SphericalHarmonics::isPeriodicWestEast() const {
    return true;
}


bool SphericalHarmonics::includesNorthPole() const {
    return true;
}


bool SphericalHarmonics::includesSouthPole() const {
    return true;
}


void SphericalHarmonics::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_SH;
    info.grid.truncation = static_cast<long>(truncation_);

    // Decision: MIR-131
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX; // Check if this is needed, why does grib_api not copy input?
    info.packing.computeLaplacianOperator = 1;
    info.packing.truncateLaplacian = 1;
    // info.packing.laplacianOperator = 0;
}


void SphericalHarmonics::fill(api::MIRJob& job) const {
    job.set("resol", truncation_);
}


size_t SphericalHarmonics::truncation() const {
    return truncation_;
}


size_t SphericalHarmonics::pentagonalResolutionTs() const {
    return Ts_;
}


void SphericalHarmonics::comparison(std::string& comparator) const {
    comparator = "spectral";
}


void SphericalHarmonics::truncate(
    size_t truncation_from,
    size_t truncation_to,
    const std::vector<double>& in,
    std::vector<double>& out ) {

    ASSERT(truncation_to != truncation_from);

    size_t insize = number_of_complex_coefficients(truncation_from) * 2;
    ASSERT(in.size() == insize);

    size_t outsize = number_of_complex_coefficients(truncation_to) * 2;
    out.resize(outsize);

    int delta = truncation_from - truncation_to;
    size_t i = 0;
    size_t j = 0;

    if (delta > 0) {
        size_t t1 = truncation_to + 1;
        for (size_t m = 0; m < t1; m++) {
            for (size_t n = m ; n < t1; n++) {
                out[i++] = in[j++];
                out[i++] = in[j++];
            }
            j += delta;
            j += delta;
        }
        ASSERT(i == outsize);
    } else {
        // Pad with zeros
        size_t t1 = truncation_to + 1;
        size_t t_ = truncation_from ;

        for (size_t m = 0; m < t1; m++) {
            for (size_t n = m ; n < t1; n++) {
                if (m > t_ || n > t_) {
                    out[i++] = 0;
                    out[i++] = 0;
                } else {
                    out[i++] = in[j++];
                    out[i++] = in[j++];
                }
            }
        }
        ASSERT(j == insize);
    }


}


const Representation *SphericalHarmonics::truncate(size_t truncation,
        const std::vector<double> &in, std::vector<double> &out) const {


    if (truncation == truncation_) {
        return 0;
    }

    truncate(truncation_, truncation, in, out);

    return new SphericalHarmonics(truncation);
}


void SphericalHarmonics::validate(const std::vector<double>& values) const {
    ASSERT(values.size() == number_of_complex_coefficients(truncation_) * 2);
}


void SphericalHarmonics::setComplexPacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX;
}


void SphericalHarmonics::setSimplePacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE;
}


namespace {
static RepresentationBuilder<SphericalHarmonics> sphericalHarmonics("sh"); // Name is what is returned by grib_api
}


}  // namespace sh
}  // namespace repres
}  // namespace mir

