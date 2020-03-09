/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_namedgrids_NamedORCA_h
#define mir_namedgrids_NamedORCA_h

#include "mir/namedgrids/NamedGrid.h"


namespace mir {
namespace namedgrids {


class NamedORCA : public NamedGrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    NamedORCA(const std::string& name);
    NamedORCA(const NamedORCA&) = delete;

    // -- Destructor

    virtual ~NamedORCA();

    // -- Convertors
    // None

    // -- Operators

    NamedORCA& operator=(const NamedORCA&) = delete;

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
    // None

    // -- Overridden methods

    virtual void print(std::ostream&) const;
    virtual size_t gaussianNumber() const;
    virtual const repres::Representation* representation() const;
    virtual const repres::Representation* representation(const util::Rotation&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace namedgrids
}  // namespace mir


#endif
