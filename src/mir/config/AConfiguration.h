/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Nov 2016


#ifndef mir_config_AConfiguration_h
#define mir_config_AConfiguration_h

#include <iosfwd>
#include <vector>
#include "eckit/filesystem/PathName.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/param/InheritParametrisation.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
}


namespace mir {
namespace config {


class AConfiguration {
public:

    // -- Exceptions
    // None

    // -- Contructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    // Lookup default parametrisation
    virtual const param::MIRParametrisation* defaults() const = 0;

protected:

    // -- Contructors
    AConfiguration();

    // -- Destructor
    virtual ~AConfiguration() {}

    // -- Members
    std::string configPath_;
    param::InheritParametrisation root_;

    // -- Methods

    // Configure (or reconfigure) using a file
    virtual void configure(const eckit::PathName& path);

    // Show useful information
    virtual void print(std::ostream&) const;

private:

    // -- Types
    // None

    // -- Members
    // None

    // -- Methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const AConfiguration& p) {
        p.print(s);
        return s;
    }

};


}  // namespace config
}  // namespace mir


#endif
