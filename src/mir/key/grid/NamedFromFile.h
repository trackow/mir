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


#pragma once

#include "mir/key/grid/NamedGrid.h"
#include "mir/param/SimpleParametrisation.h"


namespace mir::key::grid {


class NamedFromFile : public NamedGrid, public param::SimpleParametrisation {
public:
    // -- Exceptions
    // None

    // -- Constructors

    NamedFromFile(const std::string& name);
    NamedFromFile(const NamedFromFile&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    NamedFromFile& operator=(const NamedFromFile&) = delete;

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

    void print(std::ostream&) const override;
    size_t gaussianNumber() const override;
    const repres::Representation* representation() const override;
    const repres::Representation* representation(const util::Rotation&) const override;

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


}  // namespace mir::key::grid
