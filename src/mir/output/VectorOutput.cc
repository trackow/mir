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

#include "mir/output/VectorOutput.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/input/VectorInput.h"
#include "eckit/exception/Exceptions.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace output {


VectorOutput::VectorOutput(MIROutput &component1, MIROutput &component2):
    component1_(component1),
    component2_(component2) {
}


VectorOutput::~VectorOutput() {
}


size_t VectorOutput::copy(const param::MIRParametrisation &param, input::MIRInput &input) {
    try {
        input::VectorInput& v = dynamic_cast<input::VectorInput&>(input);
        size_t size = 0;
        size += component1_.copy(param, v.component1_);
        size += component2_.copy(param, v.component2_);
        return size;

    } catch (std::bad_cast &) {
        std::ostringstream os;
        os << "VectorOutput::copy() not implemented for input of type: " << input;
        throw eckit::SeriousBug(os.str());
    }
}

size_t VectorOutput::save(const param::MIRParametrisation &param, input::MIRInput &input, data::MIRField &field) {

    ASSERT(field.dimensions() == 2);

    data::MIRField u(field.representation(), field.hasMissing(), field.missingValue());
    u.update(field.direct(0), 0);

    data::MIRField v(field.representation(), field.hasMissing(), field.missingValue());
    v.update(field.direct(1), 0);

    size_t size = 0;

    size += component1_.save(param, input, u);
    size += component2_.save(param, input, v);

    return size;
}


bool VectorOutput::sameAs(const MIROutput& other) const {
    const VectorOutput* o = dynamic_cast<const VectorOutput*>(&other);
    return o && component1_.sameAs(o->component1_) && component2_.sameAs(o->component2_);
}


void VectorOutput::print(std::ostream &out) const {
    out << "VectorOutput[" << component1_ << "," << component2_ << "]";
}
}  // namespace output
}  // namespace mir

