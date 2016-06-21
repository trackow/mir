/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015

#include "mir/method/Nearest.h"

#include <string>
#include <limits>
#include <vector>

#include "eckit/log/Timer.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/ETA.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Seconds.h"

#include "mir/method/GridSpace.h"
#include "mir/util/PointSearch.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/log/MIR.h"


namespace mir {
namespace method {

//----------------------------------------------------------------------------------------------------------------------

namespace {
enum { LON=0, LAT=1 };
}

Nearest::Nearest(const param::MIRParametrisation &param) :
    MethodWeighted(param),
    epsilon_(std::numeric_limits<double>::epsilon()) {

    param.get("epsilon", epsilon_);

}


Nearest::~Nearest() {
}


const char *Nearest::name() const {
    return  "k-nearest";
}


void Nearest::hash(eckit::MD5 &md5) const {
    MethodWeighted::hash(md5);
    md5 << epsilon_;
}


void Nearest::assemble(context::Context& ctx, WeightMatrix &W, const GridSpace& in, const GridSpace& out) const {

    eckit::TraceTimer<MIR> timer("Nearest::assemble");
    eckit::Log::trace<MIR>() << "Nearest::assemble" << std::endl;

    const size_t nclosest = this->nclosest();

    const util::PointSearch sptree(in);

    const atlas::grid::Domain& inDomain = in.grid().domain();

    atlas::array::ArrayView<double, 2> ocoords = out.coordsXYZ();
    atlas::array::ArrayView<double, 2> olonlat = out.coordsLonLat();

    const size_t out_npts = out.grid().npts();
    double nearest = 0;
    double push_back = 0;

    // init structure used to fill in sparse matrix
    std::vector<WeightMatrix::Triplet > weights_triplets;
    weights_triplets.reserve(out_npts * nclosest);
    eckit::Log::trace<MIR>() << "Reserve " << eckit::BigNum(out_npts * nclosest) << std::endl;

    std::vector<util::PointSearch::PointValueType> closest;

    std::vector<double> weights;
    weights.reserve(nclosest);

    for (size_t ip = 0; ip < out_npts; ++ip) {

        if (ip && (ip % 50000 == 0)) {
            double rate = ip / timer.elapsed();
            eckit::Log::trace<MIR>() << eckit::BigNum(ip) << " ..."  << eckit::Seconds(timer.elapsed())
                               << ", rate: " << rate << " points/s, ETA: "
                               << eckit::ETA( (out_npts - ip) / rate )
                               << std::endl;

            eckit::Log::trace<MIR>() << "Nearest: " << nearest << ", Push back:" << push_back << std::endl;
            sptree.statsPrint(eckit::Log::trace<MIR>(), false);
            eckit::Log::trace<MIR>() << std::endl;
            sptree.statsReset();
            nearest = push_back = 0;
        }

        if (!inDomain.contains(olonlat[ip][LON], olonlat[ip][LAT])) {
            continue;
        }

        // get the reference output point
        eckit::geometry::Point3 p(ocoords[ip].data());

        // find the closest input points to this output
        double t = timer.elapsed();
        sptree.closestNPoints(p, nclosest, closest);
        nearest += timer.elapsed() - t;

        const size_t npts = closest.size();

        // then calculate the nearest neighbour weights
        weights.resize(npts, 0.0);

        // sum all calculated weights for normalisation
        double sum = 0.0;

        for (size_t j = 0; j < npts; ++j) {
            // one of the closest points
            eckit::geometry::Point3 np = closest[j].point();

            // calculate distance squared and weight
            const double d2 = eckit::geometry::Point3::distance2(p, np);
            weights[j] = 1. / (1. + d2);

            // also work out the total
            sum += weights[j];
        }

        ASSERT(sum > 0.0);

        // now normalise all weights according to the total
        for (size_t j = 0; j < npts; ++j) {
            weights[j] /= sum;
        }

        // insert the interpolant weights into the global (sparse) interpolant matrix
        for (int i = 0; i < npts; ++i) {
            size_t index = closest[i].payload();
            double t = timer.elapsed();
            weights_triplets.push_back(WeightMatrix::Triplet(ip, index, weights[i]));
            push_back += timer.elapsed() - t;

        }
    }

    // fill-in sparse matrix
    W.setFromTriplets(weights_triplets);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace method
}  // namespace mir

