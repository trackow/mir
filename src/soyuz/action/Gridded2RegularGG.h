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


#ifndef Gridded2RegularGG_H
#define Gridded2RegularGG_H

#include "soyuz/action/Gridded2GriddedInterpolation.h"

namespace mir {
namespace action {


class Gridded2RegularGG : public Gridded2GriddedInterpolation {
  public:

// -- Exceptions
    // None

// -- Contructors

    Gridded2RegularGG(const param::MIRParametrisation&);

// -- Destructor

    virtual ~Gridded2RegularGG(); // Change to virtual if base class

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

    void print(std::ostream&) const; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    Gridded2RegularGG(const Gridded2RegularGG&);
    Gridded2RegularGG& operator=(const Gridded2RegularGG&);

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods

    // From Gridded2GriddedInterpolation
    virtual repres::Representation* outputRepresentation(const repres::Representation* inputRepres) const;

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const Gridded2RegularGG& p)
    //	{ p.print(s); return s; }

};


}  // namespace action
}  // namespace mir
#endif

