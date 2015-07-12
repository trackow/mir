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


#ifndef Sh2Octahedral_H
#define Sh2Octahedral_H

#include "mir/action/transform/Sh2GriddedTransform.h"

namespace mir {
namespace action {


class Sh2Octahedral : public Sh2GriddedTransform {
  public:

// -- Exceptions
    // None

// -- Contructors

    Sh2Octahedral(const param::MIRParametrisation&);

// -- Destructor

    virtual ~Sh2Octahedral(); // Change to virtual if base class

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

    Sh2Octahedral(const Sh2Octahedral&);
    Sh2Octahedral& operator=(const Sh2Octahedral&);

// -- Members

    size_t N_;

// -- Methods
    // None

// -- Overridden methods

    // From Gridded2GriddedInterpolation
    virtual const repres::Representation* outputRepresentation(const repres::Representation* inputRepres) const;

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const Sh2Octahedral& p)
    //	{ p.print(s); return s; }

};


}  // namespace action
}  // namespace mir
#endif

