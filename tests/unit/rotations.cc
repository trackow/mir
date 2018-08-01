/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iostream>
#include <string>
#include <vector>

#include "eckit/geometry/Point2.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "mir/api/Atlas.h"
#include "mir/repres/latlon/RotatedLL.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Increments.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace tests {
namespace unit {


struct test_t {
    test_t(const util::Increments& increments,
           const util::Rotation& rotation,
           const util::BoundingBox& bbox,
           bool includesNorthPole = false,
           bool includesSouthPole = false) :
        increments_(increments),
        rotation_(rotation),
        bbox_(bbox),
        includesNorthPole_(includesNorthPole),
        includesSouthPole_(includesSouthPole) {
    }

    const util::Increments increments_;
    const util::Rotation rotation_;
    const util::BoundingBox bbox_;
    const bool includesNorthPole_;
    const bool includesSouthPole_;
};


CASE("MIR-282") {
    using eckit::geometry::Point2;
    using atlas::PointLonLat;
    using util::Increments;
    using util::Rotation;
    using util::BoundingBox;


    auto& log = eckit::Log::info();
    auto old = log.precision(16);


    std::vector<test_t> tests {
        test_t(Increments(0.25, 0.25), Rotation(-35.,   0.), BoundingBox(12, -14.5, -17.25,  16.25)),
        test_t(Increments(1., 1.),     Rotation(-90.,   0.), BoundingBox(), true, true),
        test_t(Increments(1., 1.),     Rotation(-75.,  15.), BoundingBox(75., -35.,  20.,    45.), true, false),
        test_t(Increments(1., 1.),     Rotation(-35.,  15.), BoundingBox(40., -55., -45.,    55.), true, false),
        test_t(Increments(1., 1.),     Rotation(-30., -15.), BoundingBox(35., -40., -40.,    50.), true, false),
        test_t(Increments(1., 1.),     Rotation(-25.,   0.), BoundingBox(40., -50., -40.,    50.), true, false),
        test_t(Increments(1., 1.),     Rotation(-15.,  45.), BoundingBox(30., -50., -30.,     5.), true, false),
        test_t(Increments(1., 1.),     Rotation(  0.,  80.), BoundingBox(50., -65., -40.,    30.), true, false),

        test_t(Increments(0.2, 0.2),   Rotation(-40.,  10.), BoundingBox(22.7, -13.6, -5.9, 21.8)),

    };


    SECTION("rotated_ll covering North/South poles") {
        for (auto& test : tests) {

            log << "check:"
                << "\n\t" << test.increments_
                << "\n\t" << test.rotation_
                << "\n\t" << test.bbox_
                << "\n\t" << "includesNorthPole? " << test.includesNorthPole_
                << "\n\t" << "includesSouthPole? " << test.includesSouthPole_
                << std::endl;

            const PointLonLat southPole(
                        test.rotation_.south_pole_longitude().normalise(Longitude::GREENWICH).value(),
                        test.rotation_.south_pole_latitude().value() );

            const atlas::util::Rotation r(southPole);

            // check bbox including poles (in the unrotated frame)
            PointLonLat NP{ r.unrotate({0., Latitude::NORTH_POLE.value()}) };
            PointLonLat SP{ r.unrotate({0., Latitude::SOUTH_POLE.value()}) };

            bool includesNorthPole = test.bbox_.contains(NP.lat(), NP.lon());
            bool includesSouthPole = test.bbox_.contains(SP.lat(), SP.lon());

            log << "check:"
                << "\n\t" << "includesNorthPole? " << includesNorthPole
                << "\n\t" << "includesSouthPole? " << includesSouthPole
                << std::endl;

            EXPECT(includesNorthPole == test.includesNorthPole_);
            EXPECT(includesSouthPole == test.includesSouthPole_);

        }
    }


    SECTION("rotated_ll contained by cropping") {
        for (auto& test : tests) {

            repres::RepresentationHandle repres(new repres::latlon::RotatedLL(
                                                    test.increments_,
                                                    test.rotation_,
                                                    test.bbox_) );

            util::BoundingBox crop(test.rotation_.rotate(test.bbox_));

            log << "check:"
                << "\n\t" "   " << *repres
                << "\n\t" "contained by"
                << "\n\t" "   " << test.bbox_
                << "\n\t" " + " << test.rotation_
                << "\n\t" " = " << crop
                << std::endl;

            bool contains = true;
            for (eckit::ScopedPtr<repres::Iterator> iter(repres->iterator()); iter->next();) {
                auto& pr = iter->pointRotated();
                if (!crop.contains(pr[0], pr[1])) {
                    log << "!crop.contains:"
                        << "\t" << iter->pointRotated()
                        << "\t" << iter->pointUnrotated()
                        << std::endl;
                    contains = false;
                }
            }
            EXPECT(contains);
        }
    }


    log.precision(old);
}


}  // namespace unit
}  // namespace tests
}  // namespace mir


int main(int argc, char **argv) {
    return eckit::testing::run_tests(argc, argv, false);
}

