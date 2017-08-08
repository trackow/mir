/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_method_distance_InverseDistanceWeightingSimple_h
#define mir_method_distance_InverseDistanceWeightingSimple_h

#include "mir/method/distance/DistanceWeighting.h"


namespace mir {
namespace method {
namespace distance {


struct InverseDistanceWeightingSimple : DistanceWeighting {
    InverseDistanceWeightingSimple(const param::MIRParametrisation&);
    void operator()(
            size_t ip,
            const eckit::geometry::Point3& point,
            const std::vector<util::PointSearch::PointValueType>& neighbours,
            std::vector<WeightMatrix::Triplet>& triplets) const;

};


}  // namespace distance
}  // namespace method
}  // namespace mir


#endif

