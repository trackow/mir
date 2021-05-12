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

#include "mir/repres/gauss/reduced/Reduced.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


class Octahedral : public Reduced {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Octahedral(size_t, const util::BoundingBox& = util::BoundingBox(), double angularPrecision = 0);

    // -- Destructor

    ~Octahedral() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    void fill(grib_info&) const override;
    void fill(api::MIRJob&) const override;
    atlas::Grid atlasGrid() const override;
    void makeName(std::ostream&) const override;
    bool sameAs(const Representation&) const override;

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

    void fill(util::MeshGeneratorParameters&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir
