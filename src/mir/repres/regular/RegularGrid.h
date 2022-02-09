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

#include <utility>  // for pair

#include "mir/repres/Gridded.h"
#include "mir/util/Atlas.h"
#include "mir/util/Shape.h"
#include "mir/util/Types.h"


namespace mir {
namespace repres {
namespace regular {


class RegularGrid : public Gridded {
public:
    // -- Types

    using LinearSpacing = ::atlas::grid::LinearSpacing;
    using PointLonLat   = ::atlas::PointLonLat;
    using Projection    = ::atlas::Projection;

    struct ij_t {
        size_t i;
        size_t j;
    };

    // -- Exceptions
    // None

    // -- Constructors

    RegularGrid(const param::MIRParametrisation&, const Projection&);
    RegularGrid(const Projection&, const util::BoundingBox&, const LinearSpacing& x, const LinearSpacing& y,
                const util::Shape&);
    RegularGrid(const RegularGrid&) = delete;

    // -- Destructor

    ~RegularGrid() override;

    // -- Convertors
    // None

    // -- Operators

    RegularGrid& operator=(const RegularGrid&) = delete;

    // -- Methods
    // None

    // -- Overridden methods

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members

    ::atlas::RegularGrid grid_;
    LinearSpacing x_;
    LinearSpacing y_;
    util::Shape shape_;
    bool xPlus_;
    bool yPlus_;
    bool firstPointBottomLeft_;

    // -- Methods

    static Projection::Spec make_proj_spec(const param::MIRParametrisation&);
    static LinearSpacing linspace(double start, double step, long num, bool plus);
    std::pair<ij_t, ij_t> minmax_ij(const util::BoundingBox&) const;

    // -- Overridden methods

    // from Representation
    bool sameAs(const Representation&) const override;

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;
    bool isPeriodicWestEast() const override;

    void fill(grib_info&) const override;
    void fill(util::MeshGeneratorParameters&) const override;
    void reorder(long scanningMode, MIRValuesVector&) const override;
    void validate(const MIRValuesVector&) const override;
    void makeName(std::ostream&) const override;
    void print(std::ostream&) const override;

    bool extendBoundingBoxOnIntersect() const override;
    bool crop(util::BoundingBox&, util::AreaCropperMapping&) const override;

    ::atlas::Grid atlasGrid() const override;
    size_t numberOfPoints() const override;

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
