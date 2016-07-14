/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/repres/gauss/reduced/Octahedral.h"

#include "atlas/grid/gaussian/OctahedralGaussian.h"
#include "mir/util/Grib.h"
#include "mir/api/MIRJob.h"


namespace mir {
namespace repres {
namespace reduced {


Octahedral::Octahedral(size_t N):
    Reduced(N) {

}


Octahedral::~Octahedral() {
}


Octahedral::Octahedral(long N, const util::BoundingBox &bbox) :
    Reduced(N, bbox) {
}


void Octahedral::fill(grib_info &info) const  {
    Reduced::fill(info);
}


void Octahedral::fill(api::MIRJob &job) const  {
    Reduced::fill(job);
    std::stringstream os;
    os << "O" << N_;
    job.set("gridname", os.str());
}


atlas::grid::Grid *Octahedral::atlasGrid() const {
    return new atlas::grid::gaussian::OctahedralGaussian(N_, atlasDomain());
}


const std::vector<long>& Octahedral::pls() const {
    if (pl_.size() == 0) {
        atlas::grid::gaussian::OctahedralGaussian grid(N_);
        pl_ = grid.pl();
    }
    return pl_;
}


}  // namespace reduced
}  // namespace repres
}  // namespace mir

