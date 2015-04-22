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

#include "soyuz/param/MIRCombinedParametrisation.h"


namespace mir {
namespace param {


MIRCombinedParametrisation::MIRCombinedParametrisation(const MIRParametrisation& user,
        const MIRParametrisation& metadata,
        const MIRParametrisation& configuration,
        const MIRParametrisation& defaults):
    user_(user),
    metadata_(metadata),
    configuration_(configuration),
    defaults_(defaults) {
}


MIRCombinedParametrisation::~MIRCombinedParametrisation() {
}


void MIRCombinedParametrisation::print(std::ostream& out) const {
    out << "MIRCombinedParametrisation[user="
        << user_ <<
        ",metadata=" << metadata_ <<
        ",configuration=" << configuration_ <<
        ",defaults=" << defaults_ <<"]";
}


bool MIRCombinedParametrisation::get(const std::string& name, std::string& value) const {

    if(name.find("user.") == 0) {
        return user_.get(name.substr(5), value);
    }

    if(name.find("field.") == 0) {
        return metadata_.get(name.substr(6), value);
    }

    // This could be a loop
    if(user_.get(name, value)) return true;
    if(metadata_.get(name, value)) return true;
    if(configuration_.get(name, value)) return true;
    if(defaults_.get(name, value)) return true;

    return false;
}


}  // namespace param
}  // namespace mir

