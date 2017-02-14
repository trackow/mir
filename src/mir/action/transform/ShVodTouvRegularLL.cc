/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Feb 2017


#include "mir/action/transform/ShVodTouvRegularLL.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/latlon/RegularLL.h"


namespace mir {
namespace action {
namespace transform {


ShVodTouvRegularLL::ShVodTouvRegularLL(const param::MIRParametrisation &parametrisation):
    ShVodTouvGridded(parametrisation) {

    std::vector<double> value;
    ASSERT(parametrisation_.get("user.grid", value));
    ASSERT(value.size() == 2);
    grid_ = util::Increments(value[0], value[1]);

}


ShVodTouvRegularLL::~ShVodTouvRegularLL() {
}


bool ShVodTouvRegularLL::sameAs(const Action& other) const {
    const ShVodTouvRegularLL* o = dynamic_cast<const ShVodTouvRegularLL*>(&other);
    return o && (grid_ == o->grid_);
}


void ShVodTouvRegularLL::print(std::ostream &out) const {
    out << "ShVodTouvRegularLL[grib=" << grid_ << "]";
}


const repres::Representation *ShVodTouvRegularLL::outputRepresentation() const {
    return new repres::latlon::RegularLL(
                util::BoundingBox(90, 0, -90, 360 - grid_.west_east()),
                grid_);
}


namespace {
static ActionBuilder< ShVodTouvRegularLL > __action("transform.sh-vod-to-uv-regular-ll");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

