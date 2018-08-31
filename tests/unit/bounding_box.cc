/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "mir/namedgrids/NamedGrid.h"
#include "mir/repres/Representation.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace tests {
namespace unit {


CASE("Representation::extendedBoundingBox") {

    using namedgrids::NamedGrid;
    using util::BoundingBox;

    auto& log = eckit::Log::info();
    auto old = log.precision(16);

    SECTION("Gaussian::extendedBoundingBox") {

        for (const auto& name : {"F16", "O16", "N16"}) {
            repres::RepresentationHandle repres = NamedGrid::lookup(name).representation();

            for (const auto& bbox : {
                     BoundingBox(90, 10, -10, 9),
                 }) {
                log << name << " extendedBoundingBox(" << "\n\t" << bbox << " ) = " << std::endl;
                BoundingBox extended = repres->extendedBoundingBox(bbox);
                log << "\t" << extended << std::endl;
            }
        }
    }

    log.precision(old);
}


} // namespace unit
} // namespace tests
} // namespace mir


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv, false);
}
