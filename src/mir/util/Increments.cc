/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "mir/util/Increments.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/Grib.h"
#include "mir/api/MIRJob.h"
#include "eckit/types/Fraction.h"

namespace mir {
namespace util {


Increments::Increments(double west_east, double south_north):
    west_east_(west_east),
    south_north_(south_north) {
    // ASSERT(west_east_ > 0);
    // ASSERT(south_north_ > 0);
}

Increments::Increments(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("west_east_increment", west_east_));
    ASSERT(parametrisation.get("south_north_increment", south_north_));
    ASSERT(west_east_ > 0);
    ASSERT(south_north_ > 0);
}

Increments::~Increments() {
}

void Increments::print(std::ostream &out) const {
    out << "Increments["
        << "west_east=" << west_east_
        << ",south_north=" << south_north_
        << "]";
}

void Increments::fill(grib_info &info) const  {
    // Warning: scanning mode not considered
    info.grid.iDirectionIncrementInDegrees = west_east_;
    info.grid.jDirectionIncrementInDegrees = south_north_;
}

void Increments::fill(api::MIRJob &job) const  {
    job.set("grid", west_east_, south_north_);
}

bool Increments::multipleOf(const Increments& other) const {
    eckit::Fraction we = eckit::Fraction(west_east_) / eckit::Fraction(other.west_east_);
    eckit::Fraction ns = eckit::Fraction(south_north_) / eckit::Fraction(other.south_north_);
    return we.integer() && ns.integer();
}

void Increments::ratio(const Increments& other, size_t& we, size_t& ns) const {
    we = static_cast<long long>(eckit::Fraction(west_east_) / eckit::Fraction(other.west_east_));
    ns = static_cast<long long>(eckit::Fraction(south_north_) / eckit::Fraction(other.south_north_));
}


}  // namespace data
}  // namespace mir

