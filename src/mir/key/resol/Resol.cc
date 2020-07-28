/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/key/resol/Resol.h"

#include <algorithm>
#include <iostream>

#include "eckit/log/Log.h"
#include "eckit/types/Fraction.h"

#include "mir/action/plan/ActionPlan.h"
#include "mir/config/LibMir.h"
#include "mir/key/grid/Grid.h"
#include "mir/key/intgrid/None.h"
#include "mir/key/truncation/Ordinal.h"
#include "mir/key/truncation/Truncation.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Assert.h"
#include "mir/util/Increments.h"
#include "mir/util/SpectralOrder.h"


namespace mir {
namespace key {
namespace resol {


Resol::Resol(const param::MIRParametrisation& parametrisation, bool forceNoIntermediateGrid) :
    parametrisation_(parametrisation) {

    // Get input truncation and a Gaussian grid number based on input (truncation) and output (grid)
    inputTruncation_ = 0;
    ASSERT(parametrisation_.fieldParametrisation().get("spectral", inputTruncation_));
    ASSERT(inputTruncation_ > 0);

    long N = std::min(getTargetGaussianNumber(), getSourceGaussianNumber());
    ASSERT(N >= 0);

    // Setup intermediate grid (before truncation)
    // NOTE: truncation can depend on the intermediate grid Gaussian number
    if (forceNoIntermediateGrid) {
        intgrid_.reset(new intgrid::None(parametrisation_, N));
    }
    else {
        std::string intgrid = "automatic";
        parametrisation_.get("intgrid", intgrid);
        intgrid_.reset(intgrid::IntgridFactory::build(intgrid, parametrisation_, N));
    }
    ASSERT(intgrid_);

    const std::string Gi = intgrid_->gridname();
    if (!Gi.empty()) {
        N = long(grid::Grid::lookup(Gi).gaussianNumber());
        ASSERT(N > 0);
    }

    // Setup truncation
    // NOTE: number takes priority over possible names
    long T = 0;
    if (parametrisation_.userParametrisation().get("truncation", T) && T > 0) {
        truncation_.reset(new truncation::Ordinal(T, parametrisation_));
    }
    else {
        std::string name = "automatic";
        parametrisation_.userParametrisation().get("truncation", name);
        truncation_.reset(truncation::TruncationFactory::build(name, parametrisation_, N));
    }
    ASSERT(truncation_);
}


void Resol::prepare(action::ActionPlan& plan) const {

    // truncate spectral coefficients
    long T = 0;
    if (truncation_->truncation(T, inputTruncation_)) {
        ASSERT(0 < T);
        plan.add("transform.sh-truncate", "truncation", T);
    }

    // transform, if specified
    const std::string gridname = intgrid_->gridname();
    if (!gridname.empty()) {

        bool vod2uv = false;
        parametrisation_.userParametrisation().get("vod2uv", vod2uv);

        const std::string transform =
            "transform." + std::string(vod2uv ? "sh-vod-to-uv-" : "sh-scalar-to-") + "namedgrid";
        plan.add(transform, "gridname", gridname);
    }
}


bool Resol::resultIsSpectral() const {
    return intgrid_->gridname().empty();
}


void Resol::print(std::ostream& out) const {
    out << "Resol[";
    auto sep = "";

    long T = 0;
    if (truncation_->truncation(T, inputTruncation_)) {
        out << sep << "truncation=" << T;
        sep = ",";
    }

    auto grid(gridname());
    if (!grid.empty()) {
        out << sep << "gridname=" << grid;
        // sep = ",";
    }

    out << "]";
}


const std::string& Resol::gridname() const {
    return intgrid_->gridname();
}


long Resol::getTargetGaussianNumber() const {
    std::vector<double> grid;
    std::string gridname;

    long N = 0;

    if (parametrisation_.userParametrisation().get("grid", grid)) {

        // get N from number of points in half-meridian (uses only grid[1] South-North increment)
        ASSERT_KEYWORD_GRID_SIZE(grid.size());
        util::Increments increments(grid[0], grid[1]);

        // use (non-shifted) global bounding box

        eckit::Fraction r = Latitude::GLOBE.fraction() / increments.south_north().latitude().fraction();

        N = long(r.integralPart() / 2);
    }
    else if (parametrisation_.userParametrisation().get("reduced", N) ||
             parametrisation_.userParametrisation().get("regular", N) ||
             parametrisation_.userParametrisation().get("octahedral", N)) {

        // get Gaussian N directly
    }
    else if (parametrisation_.userParametrisation().get("gridname", gridname)) {

        // get Gaussian N given a gridname
        N = long(grid::Grid::lookup(gridname).gaussianNumber());
    }
    else if (parametrisation_.userParametrisation().has("griddef") ||
             parametrisation_.userParametrisation().has("latitudes") ||
             parametrisation_.userParametrisation().has("longitudes")) {

        // hardcoded
        N = 64;
        eckit::Log::debug<LibMir>() << "Resol::getTargetGaussianNumber: setting N=" << N << " (hardcoded!)"
                                    << std::endl;
    }

    ASSERT(N >= 0);
    return N;
}


long Resol::getSourceGaussianNumber() const {

    // Set Gaussian N
    std::unique_ptr<util::SpectralOrder> spectralOrder(util::SpectralOrderFactory::build("cubic"));
    ASSERT(spectralOrder);

    const long N = spectralOrder->getGaussianNumberFromTruncation(inputTruncation_);
    ASSERT(N >= 0);
    return N;
}


}  // namespace resol
}  // namespace key
}  // namespace mir
