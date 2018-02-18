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

#include "mir/action/io/Save.h"

#include <iostream>

#include "mir/output/MIROutput.h"
#include "mir/input/MIRInput.h"


namespace mir {
namespace action {

Save::Save(const param::MIRParametrisation &parametrisation, input::MIRInput &input, output::MIROutput &output):
    Action(parametrisation),
    input_(input),
    output_(output) {
}

Save::~Save() {
}

bool Save::sameAs(const Action& other) const {
    const Save* o = dynamic_cast<const Save*>(&other);
    return o && input_.sameAs(o->input_) && output_.sameAs(o->output_)
           && o->output_.sameParametrisation(parametrisation_, o->parametrisation_);
}

void Save::print(std::ostream &out) const {
    out << "Save[";
    if (output_.printParametrisation(out, parametrisation_)) {
        out << ",";
    }
    out << "output=" << output_ << "]";
}

void Save::custom(std::ostream &out) const {
    out << "Save[";
    if (output_.printParametrisation(out, parametrisation_)) {
        out << ",";
    }
    out << "output=...]";
}

void Save::execute(context::Context & ctx) const {
    output_.save(parametrisation_, ctx);
}

const char* Save::name() const {
    return "Save";
}

}  // namespace action
}  // namespace mir

