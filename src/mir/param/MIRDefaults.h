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


#ifndef MIRDefaults_H
#define MIRDefaults_H

#include <string>

#include "mir/param/SimpleParametrisation.h"
#include "mir/util/ParserConsumer.h"


namespace mir {
namespace param {


class MIRDefaults : public SimpleParametrisation, public util::ParserConsumer {
  public:

// -- Exceptions
    // None

// -- Contructors



// -- Convertors
    // None

// -- Operators
    // None

// -- Methods
    // None

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods

    static const MIRDefaults& instance();

  protected:

    MIRDefaults();

// -- Destructor

    ~MIRDefaults(); // Change to virtual if base class
// -- Members
    // None

// -- Methods

    // void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    MIRDefaults(const MIRDefaults&);
    MIRDefaults& operator=(const MIRDefaults&);

// -- Members

// -- Methods
    // None

// -- Overridden methods

    // From MIRParametrisation
    virtual void print(std::ostream&) const;

    // From MIRParametrisation and ParserConsumer
    virtual void store(const std::string& name, const char* value);
    virtual void store(const std::string& name, const std::string& value);
    virtual void store(const std::string& name, bool value);
    virtual void store(const std::string& name, long value);
    virtual void store(const std::string& name, double value);

    virtual void scope(const std::string& name);

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const MIRDefaults& p)
    //  { p.print(s); return s; }

};


}  // namespace param
}  // namespace mir
#endif

