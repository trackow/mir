/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef mir_method_AddPoint_h
#define mir_method_AddPoint_h


namespace eckit {
namespace geometry {
class LLPoint2;
}
}
namespace atlas {
namespace mesh {
class Mesh;
}
}


namespace mir {     // actually should be namespace atlas::mesh::actions
namespace method {  // ...


/// Adds points to the mesh
class AddPoint {
public:
    void operator ()(atlas::mesh::Mesh& mesh, const eckit::geometry::LLPoint2& point) const;
};


}  // namespace method
}  // namespace mir


#endif
