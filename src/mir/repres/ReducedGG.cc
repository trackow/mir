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

#include "mir/repres/ReducedGG.h"

#include <iostream>

#include "atlas/Grid.h"
#include "atlas/grids/grids.h"
#include "atlas/grids/ReducedGaussianGrid.h"
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"

namespace mir {
namespace repres {


ReducedGG::ReducedGG(const param::MIRParametrisation &parametrisation) {
    ASSERT(parametrisation.get("N", N_));
    ASSERT(parametrisation.get("pl", pl_));
}

ReducedGG::ReducedGG(size_t N_):
    N_(N_) {

}

ReducedGG::~ReducedGG() {
}

ReducedGG::ReducedGG(long N, const std::vector<long> &pl, const util::BoundingBox &bbox):
    N_(N),
    pl_(pl),
    bbox_(bbox) {

}

Representation *ReducedGG::clone() const {
    return new ReducedGG(N_, pl_, bbox_);
}

void ReducedGG::print(std::ostream &out) const {
    out << "ReducedGG[N" << N_ << "]";
}

void ReducedGG::fill(grib_info &info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REDUCED_GG;
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

atlas::Grid *ReducedGG::atlasGrid() const {
    if (pl_.size() > 0) {
        // FIXME: ask atlas to support long instead of int
        std::vector<int> pl(pl_.size());
        for (size_t i = 0; i < pl_.size(); i++) {
            pl[i] = pl_[i];
        }
        return new atlas::grids::ReducedGaussianGrid(N_, &pl[0]);
    } else {
        eckit::StrStream os;
        os << "rgg.N" << N_ << eckit::StrStream::ends;
        return atlas::Grid::create(std::string(os));
    }
}

namespace {
static RepresentationBuilder<ReducedGG> reducedGG("reduced_gg"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

