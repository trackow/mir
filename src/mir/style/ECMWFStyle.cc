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


#include "mir/style/ECMWFStyle.h"

#include <iostream>
#include <set>
#include "eckit/exception/Exceptions.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/namedgrids/NamedGrid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/style/IntermediateGrid.h"
#include "mir/style/SpectralOrder.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/DeprecatedFunctionality.h"
#include "mir/util/Increments.h"


namespace mir {
namespace style {


namespace {
static MIRStyleBuilder<ECMWFStyle> __style("ecmwf");

struct DeprecatedStyle : ECMWFStyle, util::DeprecatedFunctionality {
    DeprecatedStyle(const param::MIRParametrisation& p) : ECMWFStyle(p), util::DeprecatedFunctionality("style 'dissemination' now known as 'ecmwf'") {}
};
static MIRStyleBuilder<DeprecatedStyle> __deprecated_style("dissemination");
}


ECMWFStyle::ECMWFStyle(const param::MIRParametrisation& parametrisation):
    MIRStyle(parametrisation) {
}


ECMWFStyle::~ECMWFStyle() {
}


void ECMWFStyle::prologue(action::ActionPlan& plan) const {

    std::string prologue;
    if (parametrisation_.get("prologue", prologue)) {
        plan.add(prologue);
    }

    if (parametrisation_.has("checkerboard")) {
        plan.add("misc.checkerboard");
    }

    if (parametrisation_.has("pattern")) {
        plan.add("misc.pattern");
    }

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.prologue", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.prologue.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }
}


void ECMWFStyle::sh2grid(action::ActionPlan& plan) const {

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.spectral", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.spectral.metadata", metadata);

        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    long truncation = getIntendedTruncation();
    if (truncation) {
        plan.add("transform.sh-truncate", "truncation", truncation);
    }

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);
    std::string transform = vod2uv? "sh-vod-to-uv-" : "sh-scalar-to-";  // completed later

    if (parametrisation_.userParametrisation().has("grid")) {

        param::RuntimeParametrisation runtime(parametrisation_);
        if (truncation) {
            runtime.set("truncation", truncation);
        }

        std::string intermediate_grid;
        parametrisation_.get("spectral-intermediate-grid", intermediate_grid);

        eckit::ScopedPtr<IntermediateGrid> grid(IntermediateGridFactory::build(intermediate_grid, runtime));
        if (grid->active()) {

            // use intermediate Gaussian grid with intended truncation
            plan.add("transform." + transform + "namedgrid", "gridname", grid.release());
            grid2grid(plan);
            return;
        }

        // don't use intermediate Gaussian grid
        plan.add("transform." + transform + "regular-ll");

        if (parametrisation_.userParametrisation().has("rotation")) {
            plan.add("interpolate.grid2rotated-regular-ll");

            bool wind = false;
            parametrisation_.userParametrisation().get("wind", wind);

            if (wind || vod2uv) {
                plan.add("filter.adjust-winds-directions");
                selectWindComponents(plan);
            }
        }
    }

    if (parametrisation_.userParametrisation().has("reduced")) {
        plan.add("transform." + transform + "reduced-gg");
    }

    if (parametrisation_.userParametrisation().has("regular")) {
        plan.add("transform." + transform + "regular-gg");
    }

    if (parametrisation_.userParametrisation().has("octahedral")) {
        plan.add("transform." + transform + "octahedral-gg");
    }

    if (parametrisation_.userParametrisation().has("pl")) {
        plan.add("transform." + transform + "reduced-gg-pl-given");
    }

    if (parametrisation_.userParametrisation().has("gridname")) {
        std::string gridname;
        ASSERT(parametrisation_.get("gridname", gridname));
        plan.add("transform." + transform + "namedgrid");
    }

    if (parametrisation_.userParametrisation().has("griddef")) {
        std::string griddef;
        ASSERT(parametrisation_.get("griddef", griddef));
        // TODO: this is temporary
        plan.add("transform." + transform + "octahedral-gg", "octahedral", 64L);
        plan.add("interpolate.grid2griddef");
    }

    if (isWindComponent()) {
        plan.add("filter.adjust-winds-scale-cos-latitude");
    }

    if (!parametrisation_.userParametrisation().has("rotation")) {
        selectWindComponents(plan);
    }

    if (parametrisation_.userParametrisation().get("formula.gridded", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.gridded.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }
}


void ECMWFStyle::sh2sh(action::ActionPlan& plan) const {

    if (parametrisation_.userParametrisation().has("truncation")) {
        plan.add("transform.sh-truncate");
    }

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.spectral", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.spectral.metadata", metadata);

        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);

    if (vod2uv) {
        plan.add("transform.sh-vod-to-UV");
    }

    selectWindComponents(plan);
}


void ECMWFStyle::grid2grid(action::ActionPlan& plan) const {

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);

    bool wind = false;
    parametrisation_.userParametrisation().get("wind", wind);

    if (parametrisation_.userParametrisation().has("pl") && parametrisation_.userParametrisation().has("rotation")) {
        throw eckit::UserError("'user.pl' is incompatible with option 'rotation'.");
    }


    const std::string userGrid =
        parametrisation_.userParametrisation().has("grid") ?         "regular-ll" :
        parametrisation_.userParametrisation().has("reduced") ?      "reduced-gg" :
        parametrisation_.userParametrisation().has("regular") ?      "regular-gg" :
        parametrisation_.userParametrisation().has("octahedral") ?   "octahedral-gg" :
        parametrisation_.userParametrisation().has("pl") ?           "reduced-gg-pl-given" :
        parametrisation_.userParametrisation().has("gridname") ?     "namedgrid" :
        parametrisation_.userParametrisation().has("griddef") ?      "griddef" :
        parametrisation_.userParametrisation().has("points") ?       "points" :
        "";

    if (userGrid.length()) {
        if (parametrisation_.userParametrisation().has("rotation")) {
            plan.add("interpolate.grid2rotated-" + userGrid);
            if (wind || vod2uv) {
                plan.add("filter.adjust-winds-directions");
                selectWindComponents(plan);
            }
        } else {
            plan.add("interpolate.grid2" + userGrid);
        }
    }
}


void ECMWFStyle::epilogue(action::ActionPlan& plan) const {

    bool globalise = false;
    parametrisation_.userParametrisation().get("globalise", globalise);

    if (globalise) {
        plan.add("filter.globalise");
    }

    if (parametrisation_.userParametrisation().has("area")) {
        plan.add("crop.area");
    }

    if (parametrisation_.userParametrisation().has("bitmap")) {
        plan.add("filter.bitmap");
    }

    if (parametrisation_.userParametrisation().has("frame")) {
        plan.add("filter.frame");
    }

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.epilogue", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.epilogue.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    std::string metadata;
    if (parametrisation_.userParametrisation().get("metadata", metadata)) {
        plan.add("set.metadata", "metadata", metadata);
    }

    std::string epilogue;
    if (parametrisation_.get("epilogue", epilogue)) {
        plan.add(epilogue);
    }
}


void ECMWFStyle::print(std::ostream& out) const {
    out << "ECMWFStyle[]";
}


bool ECMWFStyle::isWindComponent() const {
    long id = 0;
    parametrisation_.get("paramId", id);

    if (id == 0) {
        return false;
    }

    const eckit::Configuration& config = LibMir::instance().configuration();
    const long id_u = config.getLong("parameter-id-u", 131);
    const long id_v = config.getLong("parameter-id-v", 132);

    return (id == id_u || id == id_v);
}


bool ECMWFStyle::selectWindComponents(action::ActionPlan& plan) const {
    bool u_only = false;
    if (parametrisation_.userParametrisation().get("u-only", u_only) && u_only) {
        plan.add("select.field", "which", long(0));
    }
    bool v_only = false;
    if (parametrisation_.userParametrisation().get("v-only", v_only) && v_only) {
        ASSERT(!u_only);
        plan.add("select.field", "which", long(1));
    }
    return (u_only || v_only);
}


long ECMWFStyle::getTargetGaussianNumber() const {
    long N = 0;

    // get N from number of points in half-meridian (uses only grid[1] South-North increment)
    std::vector<double> grid;
    if (parametrisation_.userParametrisation().get("grid", grid)) {
        ASSERT(grid.size() == 2);
        util::Increments increments(grid[0], grid[1]);

        // use (non-shifted) global bounding box
        util::BoundingBox bbox;
        increments.globaliseBoundingBox(bbox, false, false);

        N = long(increments.computeNj(bbox) - 1) / 2;
        return N;
    }

    // get Gaussian N directly
    if (parametrisation_.userParametrisation().get("reduced", N) ||
        parametrisation_.userParametrisation().get("regular", N) ||
        parametrisation_.userParametrisation().get("octahedral", N)) {
        return N;
    }

    // get Gaussian N given a gridname
    std::string gridname;
    if (parametrisation_.userParametrisation().get("gridname", gridname)) {
        N = long(namedgrids::NamedGrid::lookup(gridname).gaussianNumber());
        return N;
    }

    std::ostringstream os;
    os << "ECMWFStyle: cannot calculate Gaussian number (N) from target grid";
    throw eckit::SeriousBug(os.str());
}


long ECMWFStyle::getIntendedTruncation() const {

    // Set truncation if manually specified
    long T = 0;
    if (parametrisation_.userParametrisation().get("truncation", T)) {
        return T;
    }

    // TODO: this is temporary, no support yet for unstuctured grids
    if (parametrisation_.has("griddef")) {
        return 63L;
    }

    long Tin = 0L;
    ASSERT(parametrisation_.fieldParametrisation().get("truncation", Tin));

    std::string spectralOrder = "linear";
    parametrisation_.userParametrisation().get("spectral-order", spectralOrder);

    eckit::ScopedPtr<SpectralOrder> order(SpectralOrderFactory::build(spectralOrder));
    ASSERT(order);

    // get truncation from points-per-latitude, limited to input
    long N = getTargetGaussianNumber();
    ASSERT(N > 0);

    T = order->getTruncationFromGaussianNumber(N);
    if (T > Tin) {
        eckit::Log::warning() << "Truncation ('" << spectralOrder << "') " << T << " limited by input truncation " << Tin << std::endl;
        return Tin;
    }
    return T;
}


void ECMWFStyle::prepare(action::ActionPlan& plan) const {

    // All the nasty logic goes there
    prologue(plan);

    size_t user_wants_gridded = 0;

    if (parametrisation_.userParametrisation().has("grid")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("reduced")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("regular")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("octahedral")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("pl")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("gridname")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("griddef")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("points")) {
        user_wants_gridded++;
    }

    ASSERT(user_wants_gridded <= 1);

    bool field_gridded  = parametrisation_.fieldParametrisation().has("gridded");
    bool field_spectral = parametrisation_.fieldParametrisation().has("spectral");

    ASSERT(field_gridded != field_spectral);


    if (field_spectral) {
        if (user_wants_gridded) {
            sh2grid(plan);
        } else {
            // "user wants spectral"
            sh2sh(plan);
        }
    }


    if (field_gridded) {

        std::string formula;
        if (parametrisation_.userParametrisation().get("formula.gridded", formula)) {
            std::string metadata;
            // paramId for the results of formulas
            parametrisation_.userParametrisation().get("formula.gridded.metadata", metadata);
            plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
        }
        grid2grid(plan);
    }

    epilogue(plan);
}


bool ECMWFStyle::postProcessingRequested(const api::MIRJob& job) const {
    static const char *force[] = {
        "accuracy",
        "bitmap",
        "checkerboard",
        "griddef",
        "points",
        "edition",
        "formula",
        "frame",
        "packing",
        "pattern",
        "vod2uv",
        0
    };

    for (size_t i = 0; force[i]; ++i) {
        if (job.has(force[i])) {
            return true;
        }
    }

    return false;
}


}  // namespace style
}  // namespace mir

