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


#ifndef Gridded2RotatedReduced_H
#define Gridded2RotatedReduced_H

#include "mir/action/Gridded2GriddedInterpolation.h"
#include "mir/util/Increments.h"
#include "mir/util/Rotation.h"

namespace mir {
namespace action {


class Gridded2RotatedReduced : public Gridded2GriddedInterpolation {
  public:

// -- Exceptions
    // None

// -- Contructors

    Gridded2RotatedReduced(const param::MIRParametrisation&);

// -- Destructor

    virtual ~Gridded2RotatedReduced(); // Change to virtual if base class

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

    Gridded2RotatedReduced(const Gridded2RotatedReduced&);
    Gridded2RotatedReduced& operator=(const Gridded2RotatedReduced&);

// -- Members
    size_t N_;
    util::Rotation rotation_;

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

    //friend ostream& operator<<(ostream& s,const Gridded2RotatedReduced& p)
    //	{ p.print(s); return s; }

};


}  // namespace action
}  // namespace mir
#endif

