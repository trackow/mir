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


#include "mir/action/transform/VodSh2uvOctahedral.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/ReducedOctahedral.h"


namespace mir {
namespace action {
namespace transform {


VodSh2uvOctahedral::VodSh2uvOctahedral(const param::MIRParametrisation& parametrisation):
    VodSh2uvGridded(parametrisation) {

    ASSERT(parametrisation_.get("user.octahedral", N_));

}


bool VodSh2uvOctahedral::sameAs(const Action& other) const {
    const VodSh2uvOctahedral* o = dynamic_cast<const VodSh2uvOctahedral*>(&other);
    return o && (N_ == o->N_);
}


VodSh2uvOctahedral::~VodSh2uvOctahedral() {
}


void VodSh2uvOctahedral::print(std::ostream& out) const {
    out << "VodSh2uvOctahedral[N=" << N_ << "]";
}


const repres::Representation* VodSh2uvOctahedral::outputRepresentation() const {
    return new repres::reduced::ReducedOctahedral(N_);
}


namespace {
static ActionBuilder< VodSh2uvOctahedral > __action("transform.vod-sh-to-uv-octahedral-gg");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

