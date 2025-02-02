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

#include <iosfwd>

#include "eckit/filesystem/PathName.h"

#include "mir/lsm/Mask.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
namespace repres {
class Representation;
}
}  // namespace mir


namespace mir::lsm {


class GribFileMask : public Mask {
public:
    // -- Exceptions
    // None

    // -- Constructors

    GribFileMask(const eckit::PathName&, const param::MIRParametrisation&, const repres::Representation&,
                 const std::string& which);

    // -- Destructor
    // None

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
    // None

    // -- Overridden methods

    bool active() const override;
    bool cacheable() const override = 0;
    void hash(eckit::MD5&) const override;
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    const eckit::PathName path_;


private:
    // -- Members

    std::vector<bool> mask_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<bool>& mask() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::lsm
