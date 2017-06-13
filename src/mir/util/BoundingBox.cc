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


#include "mir/util/BoundingBox.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"
#include "mir/util/Increments.h"
#include "eckit/types/Fraction.h"


namespace mir {
namespace util {





BoundingBox::BoundingBox() :
    north_(90), west_(0), south_(-90), east_(360) {
    normalise();
}


BoundingBox::BoundingBox(const eckit::Fraction& north, const eckit::Fraction& west, const eckit::Fraction& south, const eckit::Fraction& east) :
    north_(north), west_(west), south_(south), east_(east) {
    normalise();
}

BoundingBox::BoundingBox(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("north", north_));
    ASSERT(parametrisation.get("west",  west_ ));
    ASSERT(parametrisation.get("south", south_));
    ASSERT(parametrisation.get("east",  east_ ));
    normalise();
}


BoundingBox::BoundingBox(const BoundingBox& other) {
    operator=(other);
}


BoundingBox::~BoundingBox() {
}


void BoundingBox::print(std::ostream &out) const {
    out << "BoundingBox["
        <<  "north=" << double(north_)
        << ",west=" << double(west_)
        << ",south=" << double(south_)
        << ",east=" << double(east_)
        << "]";
}



const double ROUNDING = 1e14;

static double rounded(double x) {
    return round(x * ROUNDING) / ROUNDING;
}


void BoundingBox::fill(grib_info &info) const  {
    // Warning: scanning mode not considered
    info.grid.latitudeOfFirstGridPointInDegrees  = rounded(north_);
    info.grid.longitudeOfFirstGridPointInDegrees = rounded(west_);
    info.grid.latitudeOfLastGridPointInDegrees   = rounded(south_);
    info.grid.longitudeOfLastGridPointInDegrees  = rounded(east_);
}


void BoundingBox::hash(eckit::MD5 &md5) const {
    md5.add(north_);
    md5.add(west_);
    md5.add(south_);
    md5.add(east_);
}


void BoundingBox::fill(api::MIRJob &job) const  {
    job.set("area", north_, west_, south_, east_);
}


void BoundingBox::normalise() {

    bool same = west_ == east_;

    ASSERT(north_ <= 90 && south_ >= -90);
    ASSERT(north_ >= south_);

    while (west_ < -180) {
        west_ += 360;
    }

    while (west_ >= 360) {
        west_ -= 360;
    }

    while (east_ < west_) {
        east_ += 360;
    }

    while ((east_  - west_ ) > 360) {
        east_ -= 360;
    }

    if (same) {
        east_ = west_;
    }

    ASSERT(west_ <= east_);
}


eckit::Fraction BoundingBox::normalise(eckit::Fraction lon) const {
    while (lon > east_) {
        lon = lon - eckit::Fraction(360);
    }

    while (lon < west_) {
        lon = lon + eckit::Fraction(360);
    }
    return lon;
}


bool BoundingBox::contains(const eckit::Fraction& lat, const eckit::Fraction& lon) const {
    const eckit::Fraction nlon = normalise(lon);
    return (lat <= north_) && (lat >= south_) && (nlon >= west_) && (nlon <= east_);
}


static size_t computeN(double first, double last, double inc) {
    ASSERT(first <= last);
    ASSERT(inc > 0);

    eckit::Fraction f(first);
    eckit::Fraction l(last);
    eckit::Fraction i(inc);

    long long n = (l - f) / i;

    return n + 1;
}

size_t BoundingBox::computeNi(const util::Increments& increments) const {

    return computeN(west_,  east_,  increments.west_east());
}

size_t BoundingBox::computeNj(const util::Increments& increments) const {
    return computeN(south_, north_, increments.south_north());
}

void BoundingBox::makeName(std::ostream& out) const {
    out << "-"
        << double(north_)
        << ":"
        << double(west_)
        << ":"
        << double(south_)
        << ":"
        << double(east_);
}



}  // namespace util
}  // namespace mir

