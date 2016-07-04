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

#include "mir/param/MIRDefaults.h"


#include <iostream>
#include <limits>
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"
#include "mir/util/Parser.h"
#include "mir/log/MIR.h"


namespace mir {
namespace param {


MIRDefaults::MIRDefaults() {
    // For demo only:

    set("style", "mars");
    set("legendre-loader", "mapped-memory");
    set("executor", "simple");

    set("interpolation", "nearest-neighbour"); // The word 'method' is used in grib
    set("caching", true);

    set("lsm.selection", "auto");
    set("lsm.interpolation", "nearest-neighbour");

    set("epsilon", std::numeric_limits<double>::epsilon());
    set("nclosest", 4L);

    set("lsm.weight.adjustment", 0.2);
    set("lsm.value.threshold", 0.5);

    // Read the rest for the file
    eckit::PathName path("~mir/etc/mir/defaults.cfg");
    if (!path.exists())  {
        return;
    }

    eckit::Log::trace<MIR>() << "Loading MIR defaults from " << path << eckit::newl;
    util::Parser parser(path);
    parser.fill(*this);

}


MIRDefaults::~MIRDefaults() {
}

const MIRDefaults& MIRDefaults::instance() {
    static MIRDefaults instance_;
    return instance_;
}

void MIRDefaults::print(std::ostream& out) const {
    out << "MIRDefaults[";
    SimpleParametrisation::print(out);
    out << "]";
}

void MIRDefaults::store(const std::string& name, const char* value) {
    eckit::Log::trace<MIR>() << "From configuration file " << name << "=[" << value << "] (string)" << eckit::newl;
    SimpleParametrisation::set(name, value);
}

void MIRDefaults::store(const std::string& name, const std::string& value) {
    eckit::Log::trace<MIR>() << "From configuration file " << name << "=[" << value << "] (string)" << eckit::newl;
    SimpleParametrisation::set(name, value);
}

void MIRDefaults::store(const std::string& name, bool value) {
    eckit::Log::trace<MIR>() << "From configuration file " << name << "=[" << value << "] (bool)" << eckit::newl;
    SimpleParametrisation::set(name, value);
}

void MIRDefaults::store(const std::string& name, long value) {
    eckit::Log::trace<MIR>() << "From configuration file " << name << "=[" << value << "] (long)" << eckit::newl;
    SimpleParametrisation::set(name, value);
}

void MIRDefaults::store(const std::string& name, double value) {
    eckit::Log::trace<MIR>() << "From configuration file " << name << "=[" << value << "] (double)" << eckit::newl;
    SimpleParametrisation::set(name, value);
}

void MIRDefaults::scope(const std::string& name) {
    NOTIMP;
}

}  // namespace param
}  // namespace mir

