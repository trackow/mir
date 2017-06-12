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


#ifndef RegularGG_H
#define RegularGG_H

#include "mir/repres/gauss/regular/Regular.h"


namespace mir {
namespace repres {
namespace regular {


class RegularGG : public Regular {
public:

    // -- Exceptions
    // None

    // -- Contructors

    RegularGG(const param::MIRParametrisation&);
    RegularGG(size_t N);
    RegularGG(size_t N, const util::BoundingBox&);

    // -- Destructor

    virtual ~RegularGG();

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

    void print(std::ostream&) const;

    // -- Overridden methods

    virtual void cropToDomain(const param::MIRParametrisation&, context::Context&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    RegularGG();

    // No copy allowed

    RegularGG(const RegularGG&);
    RegularGG& operator=(const RegularGG&);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    virtual const Gridded* cropped(const util::BoundingBox &bbox) const;
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation& other) const;

    //virtual void validate(const std::vector<double>&) const;


    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const RegularGG& p)
    // { p.print(s); return s; }

};


}  // namespace regular
}  // namespace repres
}  // namespace mir
#endif

