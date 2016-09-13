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


#ifndef MIRConfiguration_H
#define MIRConfiguration_H

#include <iosfwd>
#include <map>
#include <string>
#include "mir/param/SimpleParametrisation.h"
#include "mir/util/ParserConsumer.h"


namespace mir {
namespace param {
class ParamClass;
}
}


namespace mir {
namespace param {


class MIRConfiguration : public util::ParserConsumer {
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

    const SimpleParametrisation* lookup(long paramId) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static const MIRConfiguration& instance();

protected:
    // -- Contructors

    MIRConfiguration();

    // -- Destructor

    ~MIRConfiguration();

    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    MIRConfiguration(const MIRConfiguration&);
    MIRConfiguration& operator=(const MIRConfiguration&);

    // -- Types

    typedef std::map< long, SimpleParametrisation* > map_t;

    // -- Members

    long current_;
    SimpleParametrisation* scope_;
    map_t settings_;

    // -- Methods
    // None

    // -- Overridden methods

    // From ParserConsumer
    virtual void store(const std::string& name, const char* value);
    virtual void store(const std::string& name, const std::string& value);
    virtual void store(const std::string& name, bool value);
    virtual void store(const std::string& name, long value);
    virtual void store(const std::string& name, double value);

    virtual void scope(const std::string& name);

    // -- Class members
    // None

    // -- Class methods

    bool flattenOnSetting(const ParamClass& settingsParams, const std::string& setting, size_t recurseLevelMax, SimpleParametrisation& paramSettings);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const MIRConfiguration& p) {
        p.print(s);
        return s;
    }

};


}  // namespace param
}  // namespace mir


#endif
