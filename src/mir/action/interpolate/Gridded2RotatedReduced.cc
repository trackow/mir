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

#include "mir/action/interpolate/Gridded2RotatedReduced.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"



#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/RotatedClassic.h"

namespace mir {
namespace action {


Gridded2RotatedReduced::Gridded2RotatedReduced(const param::MIRParametrisation &parametrisation):
    Gridded2GriddedInterpolation(parametrisation) {
    ASSERT(parametrisation_.get("user.reduced", N_));

    std::vector<double> value;
    ASSERT(parametrisation_.get("user.rotation", value));
    ASSERT(value.size() == 2);

    rotation_ = util::Rotation(value[0], value[1]);
}


Gridded2RotatedReduced::~Gridded2RotatedReduced() {
}


void Gridded2RotatedReduced::print(std::ostream &out) const {
    out << "Gridded2RotatedReduced[N=" << N_ << ",rotation=" << rotation_ << "]";
}


const repres::Representation *Gridded2RotatedReduced::outputRepresentation(const repres::Representation *inputRepres) const {
    return new repres::reduced::RotatedClassic(N_, util::BoundingBox(), rotation_);
}


namespace {
static ActionBuilder< Gridded2RotatedReduced > grid2grid("interpolate.grid2rotated-reduced-gg");
}


}  // namespace action
}  // namespace mir

