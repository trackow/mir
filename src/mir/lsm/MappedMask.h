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


#ifndef MappedMask_H
#define MappedMask_H

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
}


namespace mir {
namespace lsm {


class MappedMask : public Mask {
public:

    // -- Exceptions
    // None

    // -- Contructors

    MappedMask(const std::string& name,
               const param::MIRParametrisation& parametrisation,
               const repres::Representation &representation,
               const std::string& which);

    // -- Destructor

    virtual ~MappedMask();

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

    virtual bool active() const;
    virtual bool cacheable() const;
    virtual void hash(eckit::MD5&) const;
    virtual void print(std::ostream&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    eckit::PathName path_;
    std::vector<bool> mask_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual const std::vector<bool>& mask() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace lsm
}  // namespace mir


#endif

