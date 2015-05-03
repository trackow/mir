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


#include "mir/param/MIRParametrisation.h"
#include "eckit/exception/Exceptions.h"

namespace mir {
namespace param {


MIRParametrisation::MIRParametrisation() {
}


MIRParametrisation::~MIRParametrisation() {
}

bool MIRParametrisation::get(const std::string &name, size_t &value) const {
    long v;
    if (get(name, v)) {
        ASSERT(v >= 0);
        value = v;
        return true;
    }
    return false;
}

MIRParametrisation& MIRParametrisation::set(const std::string& name, const char* value) {
    NOTIMP;
}
MIRParametrisation& MIRParametrisation::set(const std::string& name, const std::string& value) {
    NOTIMP;
}
MIRParametrisation& MIRParametrisation::set(const std::string& name, bool value) {
    NOTIMP;
}
MIRParametrisation& MIRParametrisation::set(const std::string& name, long value) {
    NOTIMP;
}
MIRParametrisation& MIRParametrisation::set(const std::string& name, double value) {
    NOTIMP;
}

MIRParametrisation& MIRParametrisation::set(const std::string& name, const std::vector<long>& value) {
    NOTIMP;
}
MIRParametrisation& MIRParametrisation::set(const std::string& name, const std::vector<double>& value) {
    NOTIMP;
}

MIRParametrisation& MIRParametrisation::clear(const std::string& name) {
    NOTIMP;
}
void MIRParametrisation::copyValuesTo(MIRParametrisation&) const {
    NOTIMP;
}

}  // namespace param
}  // namespace mir

