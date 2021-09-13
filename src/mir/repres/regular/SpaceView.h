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


#pragma once

#include "mir/repres/regular/RegularGrid.h"


namespace mir {
namespace repres {
namespace regular {


class SpaceView : public RegularGrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    SpaceView(const param::MIRParametrisation&);
    SpaceView(const SpaceView&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    SpaceView& operator=(const SpaceView&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void fill(grib_info&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace regular
}  // namespace repres
}  // namespace mir
