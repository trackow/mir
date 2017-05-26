/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   May 2016


#ifndef mir_method_GridSpace_h
#define mir_method_GridSpace_h

#include <vector>
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"
#include "atlas/array/Array.h"
#include "mir/util/Domain.h"


namespace atlas {
class Grid;
class Mesh;
}
namespace mir {
namespace method {
class MethodWeighted;
}
}


namespace mir {
namespace method {


class GridSpace : private eckit::NonCopyable {
public:

    // -- Contructors

    GridSpace(const atlas::Grid&, const MethodWeighted&);

    // -- Methods

    const util::Domain& domain() const;
    const atlas::Grid& grid() const;
    atlas::Mesh& mesh() const;
    const atlas::array::Array& coordsLonLat() const;
    const atlas::array::Array& coordsXYZ() const;

private:

    // -- Members

    util::Domain domain_;
    const MethodWeighted& method_;
    const atlas::Grid& grid_;
    mutable atlas::Mesh* mesh_;
    mutable eckit::ScopedPtr< atlas::array::Array > coordsLonLat_;
    mutable eckit::ScopedPtr< atlas::array::Array > coordsXYZ_;

};


}  // namespace method
}  // namespace mir


#endif

