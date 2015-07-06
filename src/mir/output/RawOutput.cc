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


#include <istream>

#include "mir/output/RawOutput.h"


namespace mir {
namespace output {


RawOutput::RawOutput(double *values, size_t count):
    values_(values),
    count_(count) {
}


RawOutput::~RawOutput() {
}


void RawOutput::copy(const param::MIRParametrisation &param, input::MIRInput &input) {
}


void RawOutput::save(const param::MIRParametrisation &param, input::MIRInput &input, data::MIRField &field) {
}


void RawOutput::print(std::ostream &out) const {
    out << "RawOutput[...]";
}


}  // namespace output
}  // namespace mir

