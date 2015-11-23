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

#include "mir/input/VODInput.h"

#include <iostream>

#include "mir/data/MIRField.h"
#include "eckit/exception/Exceptions.h"

namespace mir {
namespace input {


VODInput::VODInput(MIRInput &vorticity, MIRInput &divergence):
    VectorInput(vorticity, divergence) {
}


VODInput::~VODInput() {
}

void VODInput::print(std::ostream &out) const {
    out << "VODInput[vorticity=" << component1_ << ", divergence=" << component2_ << "]";
}

}  // namespace input
}  // namespace mir

