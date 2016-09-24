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

#include "mir/style/DisseminationStyle.h"

#include <iostream>

#include "mir/param/MIRParametrisation.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/style/AutoGaussian.h"
#include "eckit/exception/Exceptions.h"

namespace mir {
namespace style {


DisseminationStyle::DisseminationStyle(const param::MIRParametrisation &parametrisation):
    ECMWFStyle(parametrisation) {

}


DisseminationStyle::~DisseminationStyle() {
}


void DisseminationStyle::print(std::ostream &out) const {
    out << "DisseminationStyle[]";
}

void DisseminationStyle::sh2grid(action::ActionPlan& plan) const {
    bool autoresol = false;
    parametrisation_.get("autoresol", autoresol);
    ASSERT(!autoresol);

    if (!parametrisation_.has("user.rotation")) {
        selectWindComponents(plan);
    }

    if (!parametrisation_.has("user.truncation")) {
        plan.add("transform.sh2octahedral-gg",
                 "octahedral", new AutoGaussian(parametrisation_));
    }

    grid2grid(plan);
}


void DisseminationStyle::sh2sh(action::ActionPlan& plan) const {

    if (parametrisation_.has("user.truncation")) {
        plan.add("transform.sh2sh");
    }

    bool vod2uv = false;
    parametrisation_.get("vod2uv", vod2uv);

    if (vod2uv) {
        plan.add("transform.vod2uv");
    }
}



// register MARS-specialized style
namespace {
static MIRStyleBuilder<DisseminationStyle> prodgen("dissemination");
}


}  // namespace style
}  // namespace mir

