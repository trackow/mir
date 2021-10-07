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
namespace detail {


/*
 * References:
 * - LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
 * - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057, Issue 6, 21.
 * June 2006)
 * - MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057 v7 e-signed. 30
 * November 2015)
 */
struct SpaceViewInternal {
    SpaceViewInternal(const param::MIRParametrisation&);
    RegularGrid::Projection projection_;
    util::BoundingBox bbox_;

    RegularGrid::LinearSpacing x() const { return {xa_, xb_, Nx_, true}; }
    RegularGrid::LinearSpacing y() const { return {ya_, yb_, Ny_, true}; }

    long Nx_;
    long Ny_;
    double xa_;
    double xb_;
    double ya_;
    double yb_;
    double LongestElementDiagonal_;
    std::vector<RegularGrid::PointLonLat> lonlat_;
};


}  // namespace detail


class SpaceView final : public detail::SpaceViewInternal, public RegularGrid {
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
    Iterator* iterator() const override;
    bool getLongestElementDiagonal(double&) const override;

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
