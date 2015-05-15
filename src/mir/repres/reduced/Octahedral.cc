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
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/reduced/Octahedral.h"

#include "atlas/grids/rgg/OctahedralRGG.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {
namespace reduced {


Octahedral::Octahedral(size_t N):
    Gaussian(N) {

}

Octahedral::~Octahedral() {
}

Octahedral::Octahedral(long N, const util::BoundingBox &bbox):
    Gaussian(N),
    bbox_(bbox) {

}

const std::vector<long>& Octahedral::pls() const {
    if (pl_.size() == 0) {
        atlas::grids::rgg::OctahedralRGG grid(N_);

        const std::vector<int>& v = grid.npts_per_lat();
        pl_.resize(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            pl_[i] = v[i];
        }
    }
    return pl_;
}

void Octahedral::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

#if 0
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_OCTAHEDRAL_GG;
#else
    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
    atlas::grids::rgg::OctahedralRGG grid(N_);

    const std::vector<long>& pl = pls();

    info.grid.pl = &pl[0];
    info.grid.pl_size = pl.size();

#endif

    info.grid.Nj = N_ * 2; // Should be PL.size()
    info.grid.N = N_;

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    size_t j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = bbox_.global() ? 1 : 0;

    // FIXME: Where are the PL set? Looks like grib_api has its own list
}

atlas::Grid *Octahedral::atlasGrid() const {
    return new atlas::grids::rgg::OctahedralRGG(N_);
}

void Octahedral::validate(const std::vector<double>& values) const {

    const std::vector<long>& pl = pls();

    size_t count = 0;
    for (size_t i = 0; i < pl.size(); i++) {
        count += pl[i];
    }
    ASSERT(values.size() == count);
}

} // namespace reduced
}  // namespace repres
}  // namespace mir

