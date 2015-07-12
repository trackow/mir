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

#include "mir/repres/latlon/ReducedLL.h"

#include <iostream>

#include "atlas/grids/LocalGrid.h"
#include "atlas/grids/ReducedLonLatGrid.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Compare.h"
#include "mir/action/misc/AreaCropper.h"

namespace mir {
namespace repres {
namespace latlon {


ReducedLL::ReducedLL(const param::MIRParametrisation &parametrisation):
    bbox_(parametrisation) {
    ASSERT(parametrisation.get("pl", pl_));
    ASSERT(parametrisation.get("Nj", Nj_));
}

ReducedLL::ReducedLL(size_t Nj, const std::vector<long> &pl, const util::BoundingBox &bbox):
    bbox_(bbox),
    pl_(pl),
    Nj_(Nj){
}

ReducedLL::~ReducedLL() {
}


void ReducedLL::print(std::ostream &out) const {
    out << "ReducedLL[bbox=" << bbox_ << "]";
}


void ReducedLL::fill(grib_info &info) const  {
    NOTIMP;
}


void ReducedLL::cropToDomain(const param::MIRParametrisation &parametrisation, data::MIRField &field) const {
    if (!globalDomain()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(field);
    }
}

bool ReducedLL::globalDomain() const {

    // FIXME: cache
    if (bbox_.north() == 90 && bbox_.south() == -90) {
        if (Nj_ == pl_.size()) {
            ASSERT(pl_.size());
            long maxpl = pl_[0];
            for (size_t i = 1; i < pl_.size(); i++) {
                maxpl = std::max(maxpl, pl_[i]);
            }

            double ew = 360.0 / maxpl;

            if (eckit::FloatCompare<double>::isApproximatelyEqual(bbox_.east() - bbox_.west() + ew, 360.)) {
                return true;
            }
        }

    }
    return false;
}

atlas::Grid *ReducedLL::atlasGrid() const {

    if ( globalDomain() ) {
        // FIXME: we are missing the distrubution of latitudes
        return new atlas::grids::ReducedLonLatGrid(pl_.size(), &pl_[0], atlas::grids::ReducedLonLatGrid::INCLUDES_POLES);
    } else {
        atlas::Domain domain(bbox_.north(), bbox_.west(), bbox_.south(), bbox_.east() );
        // FIXME: we are missing the distrubution of latitudes
        return new atlas::grids::ReducedLonLatGrid(pl_.size(), &pl_[0], atlas::grids::ReducedLonLatGrid::INCLUDES_POLES, domain);
    }
}


void ReducedLL::validate(const std::vector<double> &values) const {
    size_t count = 0;
    for (size_t i = 0; i < pl_.size(); i++) {
        count += pl_[i];
    }
    ASSERT(values.size() == count);
}

namespace {
static RepresentationBuilder<ReducedLL> reducedLL("reduced_ll"); // Name is what is returned by grib_api
}

}  // namespace latlon
}  // namespace repres
}  // namespace mir

