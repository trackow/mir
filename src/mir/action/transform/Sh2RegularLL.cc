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

#include "mir/action/transform/Sh2RegularLL.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/latlon/RegularLL.h"


namespace mir {
namespace action {


Sh2RegularLL::Sh2RegularLL(const param::MIRParametrisation &parametrisation):
    Sh2GriddedTransform(parametrisation) {
    std::vector<double> value;

    ASSERT(parametrisation_.get("user.grid", value));
    ASSERT(value.size() == 2);

    grid_ = util::Increments(value[0], value[1]);
}


Sh2RegularLL::~Sh2RegularLL() {
}


void Sh2RegularLL::print(std::ostream &out) const {
    out << "Sh2RegularLL[grib=" << grid_ << "]";
}


const repres::Representation *Sh2RegularLL::outputRepresentation(const repres::Representation *inputRepres) const {
    return new repres::latlon::RegularLL(
               util::BoundingBox(90, 0, -90, 360 - grid_.west_east()),
               grid_);
}


namespace {
static ActionBuilder< Sh2RegularLL > grid2grid("transform.sh2regular-ll");
}


}  // namespace action
}  // namespace mir

