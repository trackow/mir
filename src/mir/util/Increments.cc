/*
 * (C) Copyright 1996- ECMWF.
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
#include "eckit/types/Fraction.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"


namespace mir {
namespace util {


namespace {


static void check(const Increments& inc) {
    ASSERT(inc.west_east().longitude() >= 0);
    ASSERT(inc.south_north().latitude() >= 0);
}


static eckit::Fraction adjust(bool up, const eckit::Fraction target, const eckit::Fraction& inc) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart();

    if (!r.integer() && (r > 0) == up) {
        n += (up ? 1 : -1);
    }

    return (n * inc);
}


}  // (anonymous namespace)


Increments::Increments() = default;


Increments::Increments(const param::MIRParametrisation& parametrisation) {
    Latitude lat;
    ASSERT(parametrisation.get("south_north_increment", lat));
    south_north_ = lat;

    Longitude lon;
    ASSERT(parametrisation.get("west_east_increment", lon));
    west_east_ = lon;

    check(*this);
}


Increments::Increments(const Increments& other) :
    west_east_(other.west_east_),
    south_north_(other.south_north_) {
    check(*this);
}


Increments::Increments(double westEastIncrement, double southNorthIncrement) :
    west_east_(westEastIncrement),
    south_north_(southNorthIncrement) {
    check(*this);
}


Increments::Increments(const LongitudeIncrement& west_east, const LatitudeIncrement& south_north) :
    west_east_(west_east),
    south_north_(south_north) {
    check(*this);
}


Increments::~Increments() = default;


bool Increments::operator==(const Increments& other) const {
    return  (west_east_.longitude() == other.west_east_.longitude()) &&
            (south_north_.latitude() == other.south_north_.latitude());
}


bool Increments::operator!=(const Increments& other) const {
    return  (west_east_.longitude() != other.west_east_.longitude()) ||
            (south_north_.latitude() != other.south_north_.latitude());
}


Increments& Increments::operator=(const Increments&) = default;


bool Increments::isPeriodic() const {
    return (Longitude::GLOBE.fraction() / west_east_.longitude().fraction()).integer();
}


bool Increments::isShifted(const BoundingBox& bbox) const {
    return isLatitudeShifted(bbox) || isLongitudeShifted(bbox);
}


void Increments::print(std::ostream& out) const {
    out << "Increments["
        << "west_east=" << west_east_.longitude()
        << ",south_north=" << south_north_.latitude()
        << "]";
}


void Increments::fill(grib_info& info) const  {
    // Warning: scanning mode not considered
    info.grid.iDirectionIncrementInDegrees = west_east_.longitude().value();
    info.grid.jDirectionIncrementInDegrees = south_north_.latitude().value();
}


void Increments::fill(api::MIRJob& job) const  {
    job.set("grid", west_east_.longitude().value(), south_north_.latitude().value());
}


void Increments::globaliseBoundingBox(BoundingBox& bbox, const PointLatLon& reference) const {
    using eckit::Fraction;

    Fraction sn = south_north_.latitude().fraction();
    Fraction we = west_east_.longitude().fraction();
    ASSERT(sn > 0);
    ASSERT(we > 0);

    Fraction shift_sn = (reference.lat().fraction() / sn).decimalPart() * sn;
    Fraction shift_we = (reference.lon().fraction() / we).decimalPart() * we;


    // Latitude limits

    Latitude n = adjust(false, Latitude::NORTH_POLE.fraction() - shift_sn, sn) + shift_sn;
    Latitude s = adjust(true,  Latitude::SOUTH_POLE.fraction() - shift_sn, sn) + shift_sn;


    // Longitude limits
    // - West for non-periodic grids is not corrected!
    // - East for periodic grids is W + 360 - increment

    Longitude w = bbox.west();
    if (isPeriodic()) {
        w = adjust(true, Longitude::GREENWICH.fraction() - shift_we, we) + shift_we;
    }

    Longitude e = adjust(false, w.fraction() + Longitude::GLOBE.fraction() - shift_we, we) + shift_we;
    if (e - w == Longitude::GLOBE) {
        e -= we;
    }


    // set bounding box
    bbox = {n, w, s, e};
}


void Increments::correctBoundingBox(BoundingBox& bbox, const PointLatLon& reference) const {
    using eckit::Fraction;

    Fraction sn = south_north_.latitude().fraction();
    Fraction we = west_east_.longitude().fraction();
    ASSERT(sn >= 0);
    ASSERT(we >= 0);


    // Latitude limits
    // - North adjusted to N = S + Nj * inc <= 90

    Latitude s = bbox.south();
    Latitude n = sn == 0 ? s : bbox.north();

    if (sn > 0) {
        Fraction shift = (reference.lat().fraction() / sn).decimalPart() * sn;

        s = adjust(true,  bbox.south().fraction() - shift, sn) + shift;

        if (bbox.south() == bbox.north()) {
            n = s;
        } else {
            n = adjust(false, bbox.north().fraction() - shift, sn) + shift;
            if (n < s) {
                n = s;
            }
        }
    }

    // Longitude limits
    // - East adjusted to E = W + Ni * inc < W + 360
    // (non-periodic grids can have 360 - inc < E - W < 360)

    Longitude w = bbox.west();
    Longitude e = we == 0 ? w : bbox.east();

    if (we > 0) {
        Fraction shift = (reference.lon().fraction() / we).decimalPart() * we;

        w = adjust(true,  bbox.west().fraction() - shift, we) + shift;
        ASSERT(bbox.west() <= w);

        if (bbox.west() == bbox.east()) {
            e = w;
        } else {
            e = adjust(false, bbox.east().fraction() - shift, we) + shift;
            ASSERT(e <= bbox.east());

            if (e < w) {
                e = w;
            } else if (e - w >= Longitude::GLOBE) {
                e -= we;
            }
        }

    }

    // set bounding box
    ASSERT(s <= n);
    ASSERT(w <= e);
    bbox = {n, w, s, e};
}


void Increments::makeName(std::ostream& out) const {
    out << "-" << west_east_.longitude().value()
        << "x" << south_north_.latitude().value();
}


bool Increments::isLatitudeShifted(const BoundingBox& bbox) const {
    auto& inc = south_north_.latitude();
    if (inc == 0) {
        return false;
    }
    return !(bbox.south().fraction() / inc.fraction()).integer();
}


bool Increments::isLongitudeShifted(const BoundingBox& bbox) const {
    auto& inc = west_east_.longitude();
    if (inc == 0) {
        return false;
    }
    return !(bbox.west().fraction() / inc.fraction()).integer();
}


}  // namespace util
}  // namespace mir

