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


#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/runtime/Context.h"

#include "mir/api/mir_config.h"

#include "mir/action/Action.h"
#include "mir/action/ActionPlan.h"

#include "mir/data/MIRField.h"
#include "mir/input/MIRInput.h"
#include "mir/logic/MIRLogic.h"
#include "mir/output/MIROutput.h"
#include "mir/param/MIRCombinedParametrisation.h"
#include "mir/param/MIRDefaults.h"

#include "mir/repres/Representation.h"

#include "mir/api/ProdgenJob.h"


namespace mir {
namespace api {


ProdgenJob::ProdgenJob() {
}


ProdgenJob::~ProdgenJob() {
}


void ProdgenJob::execute(input::MIRInput& input, output::MIROutput& output) const {
    // Optimisation: nothing to do, usefull for MARS
    if (size() == 0) {
        eckit::Log::info() << "Nothing to do (no request)" << std::endl;
        output.copy(*this, input);
        return;
    }


    // Accroding to c++11, this should be thread safe (assuming contructors are thread safe as well)

    const param::MIRParametrisation& defaults = param::MIRDefaults::instance();
    eckit::Log::info() << "Defaults: " << defaults << std::endl;

    eckit::Timer timer("ProdgenJob::execute");

    eckit::Log::info() << "ProdgenJob::execute: ";
    mirToolCall(eckit::Log::info());
    eckit::Log::info()<< std::endl;

    eckit::Log::info() << "          Input: " << input << std::endl;
    eckit::Log::info() << "         Output: " << output << std::endl;

    const param::MIRParametrisation& metadata = input.parametrisation();

    if (matches(metadata)) {
        eckit::Log::info() << "Nothing to do (field matches)" << std::endl;
        output.copy(*this, input);
        return;
    }

    param::MIRCombinedParametrisation combined(*this, metadata, defaults);
    eckit::Log::info() << "Combined parametrisation: " << combined << std::endl;

    eckit::ScopedPtr< logic::MIRLogic > logic(logic::MIRLogicFactory::build(combined));

    eckit::Log::info() << "Logic: " << *logic << std::endl;

    action::ActionPlan plan(combined);
    logic->prepare(plan);

    eckit::Log::info() << "Action plan is: " << plan << std::endl;
    if(plan.empty()) {
        eckit::Log::info() << "Nothing to do (empty plan)" << std::endl;
        output.copy(*this, input);
        return;
    }

    eckit::ScopedPtr< data::MIRField > field(input.field());
    eckit::Log::info() << "Field is " << *field << std::endl;


    plan.execute(*field);

    output.save(combined, input, *field);
}


void ProdgenJob::print(std::ostream& out) const {
    if (eckit::format(out) == eckit::Log::applicationFormat) {
        out << "mir_tool ";
        SimpleParametrisation::print(out);
        out << " in.grib out.grib";
    } else {
        out << "ProdgenJob[";
        SimpleParametrisation::print(out);
        out << "]";
    }
}

ProdgenJob& ProdgenJob::clear(const std::string& name) {
    eckit::Log::info() << "************* ProdgenJob::clear [" << name << "]" << std::endl;
    SimpleParametrisation::clear(name);
    return *this;
}
ProdgenJob& ProdgenJob::set(const std::string& name, const std::string& value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (string)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}

ProdgenJob& ProdgenJob::set(const std::string& name, const char* value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (char)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}

ProdgenJob& ProdgenJob::set(const std::string& name, bool value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (bool)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}


ProdgenJob& ProdgenJob::set(const std::string& name, long value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (long)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}


ProdgenJob& ProdgenJob::set(const std::string& name, double value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (double)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}

ProdgenJob& ProdgenJob::set(const std::string& name, param::DelayedParametrisation* value) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << value << "] (delayed)" << std::endl;
    SimpleParametrisation::set(name, value);
    return *this;
}

ProdgenJob& ProdgenJob::set(const std::string& name, double v1, double v2) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] = [" << v1 << ", "  << v2 << "] (double)" << std::endl;
    std::vector<double> v(2);
    v[0] = v1;
    v[1] = v2;
    SimpleParametrisation::set(name, v);
    return *this;
}

ProdgenJob& ProdgenJob::set(const std::string& name, double v1, double v2, double v3, double v4) {
    eckit::Log::info() << "************* ProdgenJob::set [" << name << "] =  [" << v1
                       << ", "  << v2 << ", "  << v3 << ", "  << v4 << "] (double)" << std::endl;
    std::vector<double> v(4);
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    v[3] = v4;
    SimpleParametrisation::set(name, v);
    return *this;
}


bool ProdgenJob::matches(const param::MIRParametrisation& metadata) const {
    static const char* force[] = { "vod2uv", "bitmap", "frame", "packing", "accuracy", 0 }; // Move to MIRLogic
    size_t i = 0;
    while (force[i]) {
        if (has(force[i])) {
            eckit::Log::info() << "ProdgenJob will perform transformation/interpolation ('" << force[i] << "'' specified)" << std::endl;
            return false;
        }
        i++;
    }

    bool ok = SimpleParametrisation::matches(metadata);
    if (!ok) {
        eckit::Log::info() << "ProdgenJob will perform transformation/interpolation" << std::endl;
    }
    return ok;
}

void ProdgenJob::mirToolCall(std::ostream& out) const {
    int fmt = eckit::format(out);
    eckit::setformat(out, eckit::Log::applicationFormat);
    out << *this;
    eckit::setformat(out, fmt);
}


// This comes grom eckit::Context
static eckit::RegisterConfigHome configs("mir",
        MIR_INSTALL_BIN_DIR,
        MIR_DEVELOPER_BIN_DIR,
        MIR_INSTALL_DATA_DIR,
        MIR_DEVELOPER_BIN_DIR);

}  // namespace api
}  // namespace mir

