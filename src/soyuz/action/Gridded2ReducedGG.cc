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

#include "soyuz/action/Gridded2ReducedGG.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace action {


Gridded2ReducedGG::Gridded2ReducedGG(const param::MIRParametrisation& parametrisation):
    Gridded2GriddedInterpolation(parametrisation) {
}


Gridded2ReducedGG::~Gridded2ReducedGG() {
}


void Gridded2ReducedGG::print(std::ostream& out) const {
    out << "Gridded2ReducedGG[]";
}


repres::Representation* Gridded2ReducedGG::outputRepresentation(const repres::Representation* inputRepres) const {
    NOTIMP;
}


namespace {
static ActionBuilder< Gridded2ReducedGG > grid2grid("interpolate.grid2reduced-gg");
}


}  // namespace action
}  // namespace mir

