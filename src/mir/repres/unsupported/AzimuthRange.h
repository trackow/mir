/*
 * (C) Copyright 1996- ECMWF.
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


#ifndef AzimuthRange_H
#define AzimuthRange_H

#include "mir/repres/Gridded.h"


namespace mir {
namespace repres {


class AzimuthRange : public Gridded {
  public:

// -- Exceptions
    // None

// -- Contructors

    AzimuthRange(const param::MIRParametrisation&);

// -- Destructor

    virtual ~AzimuthRange(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods

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

    AzimuthRange();

// No copy allowed

    AzimuthRange(const AzimuthRange&);
    AzimuthRange& operator=(const AzimuthRange&);

// -- Members


// -- Methods
    // None


// -- Overridden methods

    virtual void fill(grib_info&) const;

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const AzimuthRange& p)
    //	{ p.print(s); return s; }

};


}  // namespace repres
}  // namespace mir
#endif

