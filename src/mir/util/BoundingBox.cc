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


#include "mir/util/BoundingBox.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"


namespace mir {
namespace util {


namespace {


static void check(const BoundingBox& bbox) {
    ASSERT(bbox.north() >= bbox.south());
    ASSERT(bbox.north() <= Latitude::NORTH_POLE);
    ASSERT(bbox.south() >= Latitude::SOUTH_POLE);

    ASSERT(bbox.east() - bbox.west() >= 0);
    ASSERT(bbox.east() - bbox.west() <= Longitude::GLOBE);
}


}  // (anonymous namespace)


BoundingBox::BoundingBox() :
    north_(Latitude::NORTH_POLE),
    west_(Longitude::GREENWICH),
    south_(Latitude::SOUTH_POLE),
    east_(Longitude::GLOBE) {
}


BoundingBox::BoundingBox(const Latitude& north,
                         const Longitude& west,
                         const Latitude& south,
                         const Longitude& east) :
    north_(north), west_(west), south_(south), east_(east) {
    normalise();
    check(*this);
}


BoundingBox::BoundingBox(const param::MIRParametrisation& parametrisation) {
    ASSERT(parametrisation.get("north", north_));
    ASSERT(parametrisation.get("west",  west_ ));
    ASSERT(parametrisation.get("south", south_));
    ASSERT(parametrisation.get("east",  east_ ));

    normalise();
    check(*this);
}


BoundingBox::BoundingBox(const BoundingBox& other) {
    operator=(other);
}


BoundingBox::~BoundingBox() = default;


void BoundingBox::print(std::ostream& out) const {
    out << "BoundingBox["
        <<  "north=" << north_
        << ",west=" << west_
        << ",south=" << south_
        << ",east=" << east_
        << "]";
}


void BoundingBox::fill(grib_info &info) const  {
    // Warning: scanning mode not considered
    info.grid.latitudeOfFirstGridPointInDegrees  = north_.value();
    info.grid.longitudeOfFirstGridPointInDegrees = west_.value();
    info.grid.latitudeOfLastGridPointInDegrees   = south_.value();
    info.grid.longitudeOfLastGridPointInDegrees  = east_.value();

    const long c = info.packing.extra_settings_count++;
    info.packing.extra_settings[c].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[c].name = "expandBoundingBox";
    info.packing.extra_settings[c].long_value = 1;
}


void BoundingBox::hash(eckit::MD5 &md5) const {
    md5.add(north_);
    md5.add(west_);
    md5.add(south_);
    md5.add(east_);
}


void BoundingBox::fill(api::MIRJob &job) const  {
    job.set("area", north_.value(), west_.value(), south_.value(), east_.value());
}


void BoundingBox::normalise() {
    if (west_ != east_) {
        Longitude eastNormalised = east_.normalise(west_);
        if (eastNormalised == west_) {
            eastNormalised += Longitude::GLOBE;
        }
        east_ = eastNormalised;
    }

    ASSERT(west_ <= east_);
    ASSERT(east_ <= west_ + Longitude::GLOBE);
}


bool BoundingBox::contains(const Latitude& lat, const Longitude& lon) const {
    return (lat <= north_) &&
           (lat >= south_) &&
            (lon.normalise(west_) <= east_);
}


bool BoundingBox::contains(const BoundingBox& other) const {
    return contains(other.north(), other.west()) &&
            contains(other.north(), other.east()) &&
            contains(other.south(), other.west()) &&
            contains(other.south(), other.east());
}


void BoundingBox::makeName(std::ostream& out) const {
    out << "-" << north_
        << ":" << west_
        << ":" << south_
        << ":" << east_;
}


BoundingBox BoundingBox::rotate(const Rotation& rotation) const {
    using eckit::geometry::Point2;

    // rotate bounding box corners and find (min, max)
    const atlas::PointLonLat southPole(
                rotation.south_pole_longitude().normalise(Longitude::GREENWICH).value(),
                rotation.south_pole_latitude().value() );

    const atlas::util::Rotation r(southPole);
    const atlas::PointLonLat p[] {
        r.rotate({west_.value(), north_.value()}),
        r.rotate({east_.value(), north_.value()}),
        r.rotate({east_.value(), south_.value()}),
        r.rotate({west_.value(), south_.value()})
    };

    Point2 min(p[0]);
    Point2 max(p[0]);
    for (size_t i = 1; i < 4; ++i) {
        min = Point2::componentsMin(min, p[i]);
        max = Point2::componentsMax(max, p[i]);
    }


    // extend by 'angle' latitude- and longitude-wise
    constexpr double angle = 0.; //0.001 ??
    ASSERT(angle >= 0);

    Latitude n = max[1] + angle > Latitude::NORTH_POLE.value() ? Latitude::NORTH_POLE : max[1] + angle;
    Latitude s = min[1] - angle < Latitude::SOUTH_POLE.value() ? Latitude::SOUTH_POLE : min[1] - angle;

    Longitude w = min[0];
    Longitude e = max[0];

    if ((Longitude::GLOBE + w - e).value() < 2. * angle) {
        e = Longitude::GLOBE + w;
    } else {
        w = min[0] - angle;
        e = max[0] + angle > (w + Longitude::GLOBE).value() ?
                    w + Longitude::GLOBE : Longitude(max[0] + angle);
    }

    return BoundingBox(n, w, s, e);
}


}  // namespace util
}  // namespace mir

