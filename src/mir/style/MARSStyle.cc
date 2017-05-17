/*
 * (C) Copyright 1996-2017 ECMWF.
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


#include "mir/style/MARSStyle.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/style/AutomaticResolution.h"
#include "mir/style/IntermediateGrid.h"


namespace mir {
namespace style {


MARSStyle::MARSStyle(const param::MIRParametrisation &parametrisation):
    ECMWFStyle(parametrisation) {

}


MARSStyle::~MARSStyle() {
}


void MARSStyle::print(std::ostream &out) const {
    out << "MARSStyle[]";
}


void MARSStyle::sh2grid(action::ActionPlan& plan) const {

    bool vod2uv = false;
    parametrisation_.get("vod2uv", vod2uv);
    std::string transform = vod2uv? "sh-vod-to-uv-" : "sh-scalar-to-";  // completed later

    std::string intermediate_grid;
    parametrisation_.get("spectral-intermediate-grid", intermediate_grid);

    if (intermediate_grid.length()) {
        eckit::ScopedPtr<param::DelayedParametrisation> intermediate_gridname(
                    IntermediateGridFactory::build(intermediate_grid, parametrisation_) );
        plan.add("transform." + transform + "namedgrid", "gridname", intermediate_gridname.get());
        grid2grid(plan);
        return;
    }

    if (parametrisation_.has("user.grid")) {

        long intermediate_gaussian = 0;
        parametrisation_.get("intermediate_gaussian", intermediate_gaussian);

        if (intermediate_gaussian) {
            plan.add("transform." + transform + "reduced-gg", "reduced", intermediate_gaussian);
            plan.add("interpolate.grid2regular-ll");
        } else {
            plan.add("transform." + transform + "regular-ll");
        }

        if (parametrisation_.has("user.rotation")) {
            plan.add("interpolate.grid2rotated-regular-ll");

            bool wind = false;
            parametrisation_.get("wind", wind);

            if (wind || vod2uv) {
                plan.add("filter.adjust-winds-directions");
                selectWindComponents(plan);
            }
        }

    }

    if (parametrisation_.has("user.reduced")) {
        plan.add("transform." + transform + "reduced-gg");
    }

    if (parametrisation_.has("user.regular")) {
        plan.add("transform." + transform + "regular-gg");
    }

    if (parametrisation_.has("user.octahedral")) {
        plan.add("transform." + transform + "octahedral-gg");
    }

    if (parametrisation_.has("user.pl")) {
        plan.add("transform." + transform + "reduced-gg-pl-given");
    }

    if (parametrisation_.has("user.gridname")) {
        std::string gridname;
        ASSERT(parametrisation_.get("gridname", gridname));
        plan.add("transform." + transform + "namedgrid");
    }

    if (parametrisation_.has("user.griddef")) {
        std::string griddef;
        ASSERT(parametrisation_.get("griddef", griddef));
        // TODO: this is temporary
        plan.add("transform." + transform + "octahedral-gg", "octahedral", 64L);
        plan.add("interpolate.grid2griddef");
    }

    if (isWindComponent()) {
        plan.add("filter.adjust-winds-scale-cos-latitude");
    }

    if (!parametrisation_.has("user.rotation")) {
        selectWindComponents(plan);
    }
}


void MARSStyle::shTruncate(action::ActionPlan& plan) const {

    bool autoresol = true;
    parametrisation_.get("autoresol", autoresol);

    if (autoresol) {
        eckit::ScopedPtr<param::DelayedParametrisation> automatic(new AutomaticResolution(parametrisation_));
        plan.add("transform.sh-truncate", "truncation", automatic.get());
    } else {
        if (parametrisation_.has("user.truncation")) {
            plan.add("transform.sh-truncate");
        }
    }
}


namespace {
static MIRStyleBuilder<MARSStyle> __style("mars");
}


}  // namespace style
}  // namespace mir

