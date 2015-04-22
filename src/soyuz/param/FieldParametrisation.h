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


#ifndef FieldParametrisation_H
#define FieldParametrisation_H

#include "soyuz/param/MIRParametrisation.h"


namespace mir {
namespace param {


class FieldParametrisation : public MIRParametrisation {
  public:

// -- Exceptions
    // None

// -- Contructors

    FieldParametrisation();

// -- Destructor

    virtual ~FieldParametrisation(); // Change to virtual if base class

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
    // None

  protected:

// -- Members
    // None

// -- Methods

    virtual void print(std::ostream&) const = 0; // Change to virtual if base class

// -- Overridden methods
    // From MIRParametrisation
    virtual bool get(const std::string& name, std::string& value) const;

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    FieldParametrisation(const FieldParametrisation&);
    FieldParametrisation& operator=(const FieldParametrisation&);

// -- Members
    // None

// -- Methods
    virtual bool lowLevelGet(const std::string& name, std::string& value) const = 0;

// -- Overridden methods

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    friend std::ostream& operator<<(std::ostream& s,const FieldParametrisation& p) {
        p.print(s);
        return s;
    }

};


}  // namespace param
}  // namespace mir
#endif

