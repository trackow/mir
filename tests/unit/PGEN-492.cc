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


#include <memory>
#include <string>

#include "eckit/testing/Test.h"

#include "mir/data/MIRField.h"
#include "mir/input/GribFileInput.h"
#include "mir/repres/Representation.h"
#include "mir/util/Domain.h"
#include "mir/util/Types.h"


namespace mir::tests::unit {


CASE("PGEN-492") {
    std::unique_ptr<input::MIRInput> input(new input::GribFileInput("stream=wave,param=swh"));
    ASSERT(input->next());

    const auto& param = input->parametrisation();

    std::string gridType;
    param.get("gridType", gridType);
    ASSERT(gridType == "reduced_ll");

    auto longitude = [&](const std::string& key) -> Longitude {
        double value = 0.;
        ASSERT(param.get(key, value));
        return value;
    };

    auto west = longitude("west");
    ASSERT(west == Longitude::GREENWICH);

    auto east = longitude("east");  // wrongly encoded, should be 360.
    EXPECT_EQUAL(east, Longitude::GLOBE);

    repres::RepresentationHandle repres(input->field().representation());
    auto domain = repres->domain();

    EXPECT(domain.isPeriodicWestEast());
}


}  // namespace mir::tests::unit


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
