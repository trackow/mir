/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/action/interpolate/Gridded2Points.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/other/UnstructuredGrid.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2Points::Gridded2Points(const param::MIRParametrisation& parametrisation):
    Gridded2UnrotatedGrid(parametrisation) {
    auto& user = parametrisation_.userParametrisation();
    auto& field = parametrisation_.fieldParametrisation();

    ASSERT(user.has("latitudes")  && field.get("latitudes",  latitudes_));
    ASSERT(user.has("longitudes") && field.get("longitudes", longitudes_));

    ASSERT(latitudes_.size() == longitudes_.size());
}


Gridded2Points::~Gridded2Points() = default;


bool Gridded2Points::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2Points*>(&other);
    return o && (latitudes_ == o->latitudes_) && (longitudes_ == o->longitudes_) && Gridded2GriddedInterpolation::sameAs(other);
}

void Gridded2Points::print(std::ostream& out) const {
    out << "Gridded2Points["
           "points=" << latitudes_.size() << ",";
    Gridded2UnrotatedGrid::print(out);
    out  << "]";
}


const repres::Representation* Gridded2Points::outputRepresentation() const {
    return new repres::other::UnstructuredGrid(latitudes_, longitudes_);
}

const char* Gridded2Points::name() const {
    return "Gridded2Points";
}


namespace {
static ActionBuilder< Gridded2Points > grid2grid("interpolate.grid2points");
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir

