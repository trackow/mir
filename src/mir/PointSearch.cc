/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"

#include "PointSearch.h"
#include <string>
#include <algorithm>
#include <functional> 
#include <math.h>
 

using eckit::grid::Point2D;

//-----------------------------------------------------------------------------

namespace mir {

//-----------------------------------------------------------------------------
  
PointSearch::PointSearch(const std::vector<Point2D>& points)
{
    eckit::Log::info() << "Build a PointSearch" << std::endl;

    /// @todo the kd tree might be stored in shared memory ?

    std::vector<IndexPoint> kd_points;
    for (unsigned int i = 0; i < points.size(); i++)
    {
        const Point2D& rpt = points[i];
        // we use the index of the point in the orignal array as the payload
        // as we need to know this in the results of searches
        kd_points.push_back(IndexPoint((double)rpt.lat_, (double)rpt.lon_, i)); 
    }

    kd_.build(kd_points.begin(), kd_points.end());

}

PointSearch::~PointSearch()
{
    eckit::Log::info() << "Destroy a PointSearch" << std::endl;
}

void PointSearch::closestNPoints(const Point2D& pt, size_t n, std::vector<Point2D>& closest, std::vector<unsigned int>& indices)
{
    /// @todo fix the signature here by defining an IndexPoint type?

    // @todo we need to be able to be more fussy about the points we request
    // e.g.          x x x x
    //
    //                  x  <-- closest 4 points to this point are the 4 above
    //                         but we need to know about points at lower lats
    //                         in order to do an interpolation
    //
    //
    //
    //            x           x
    //

    eckit::KDTree<IndexPoint>::NodeList nn = kd_.kNearestNeighbours(IndexPoint(pt.lat_, pt.lon_, 0), n);
    
    //std::sort (nn.begin(), nn.end());

    closest.resize(0);
    indices.resize(0);

    for (eckit::KDTree<IndexPoint>::NodeList::iterator it = nn.begin(); it != nn.end(); ++it)
    {
        closest.push_back(Point2D(it->point().x(0), it->point().x(1)));
        indices.push_back(it->point().payload_);
    }

}

} // namespace mir
