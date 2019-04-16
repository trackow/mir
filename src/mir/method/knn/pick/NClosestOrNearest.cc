/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/pick/NClosestOrNearest.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {
namespace knn {
namespace pick {


NClosestOrNearest::NClosestOrNearest(const param::MIRParametrisation& param) {
    nClosest_ = 4;
    param.get("nclosest", nClosest_);
    ASSERT(nClosest_ > 0);

    distanceTolerance_ = 1.;
    param.get("distance-tolerance", distanceTolerance_);
    ASSERT(distanceTolerance_ >= 0.);

    distanceTolerance2_ = distanceTolerance_ * distanceTolerance_;
}


void NClosestOrNearest::pick(const search::PointSearch& tree,
                             const eckit::geometry::Point3& p,
                             Pick::neighbours_t& closest) const {
    tree.closestNPoints(p, nClosest_, closest);
    ASSERT(closest.size() == nClosest_);

    // if closest and farthest nb. are at the same distance, other points can
    // also be (like near poles), so we return all points inside radius
    if (nClosest_ > 1) {
        auto nearest2 = Point3::distance2(p, closest.front().point());
        auto farthest2 = Point3::distance2(p, closest.back().point());
        if (eckit::types::is_approximately_equal(nearest2, farthest2, distanceTolerance2_)) {
            auto radius = std::sqrt(farthest2) + distanceTolerance_;
            tree.closestWithinRadius(p, radius, closest);
            ASSERT(closest.size() >= nClosest_);
        }
    }
}


size_t NClosestOrNearest::n() const {
    return nClosest_;
}


bool NClosestOrNearest::sameAs(const Pick& other) const {
    auto o = dynamic_cast<const NClosestOrNearest*>(&other);
    return o
            && nClosest_ == o->nClosest_
            && eckit::types::is_approximately_equal(distanceTolerance_, o->distanceTolerance_);
}


void NClosestOrNearest::print(std::ostream& out) const {
    out << "NClosestOrNearest["
            "nclosest=" << nClosest_
        << ",distanceTolerance=" << distanceTolerance_
        << "]";
}


void NClosestOrNearest::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static PickBuilder<NClosestOrNearest> __pick1("nclosest-or-nearest");
static PickBuilder<NClosestOrNearest> __pick2("k");


}  // namespace pick
}  // namespace knn
}  // namespace method
}  // namespace mir

