/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @date Oct 2013

#ifndef mir_PointSearch_H
#define mir_PointSearch_H


#include "eckit/container/KDMemory.h"
#include "eckit/container/KDTree.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/geometry/Point3.h"
#include "atlas/grid/Grid.h"
#include "atlas/grid/PointIndex3.h"


//-----------------------------------------------------------------------------


namespace mir {

//-----------------------------------------------------------------------------

class PointSearch : private eckit::NonCopyable {


    typedef atlas::PointIndex3::Point Point;
    typedef atlas::PointIndex3::Value ValueType;

    typedef atlas::PointIndex3::iterator iterator;
    typedef eckit::geometry::Point3 PointType;

public:

    PointSearch( const std::vector< Point >& ipts ); 
 
    PointSearch( atlas::Mesh& mesh ); 
 
    virtual ~PointSearch(){ delete tree_; } 

public: // methods


    /// Finds closest N points to an input point
    void closestNPoints(const PointType& pt, size_t n, std::vector<ValueType>& closest);
  
protected:
    
    atlas::PointIndex3* tree_;


private:

    void init(const std::vector<PointType>& points);
};

} // namespace mir

#endif
