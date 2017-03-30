/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @date Mar 2017


#ifndef mir_action_transform_mapping_Quartic_h
#define mir_action_transform_mapping_Quartic_h

#include <iosfwd>
#include <string>
#include "eckit/memory/NonCopyable.h"
#include "mir/action/transform/mapping/Mapping.h"


namespace mir {
namespace action {
namespace transform {
namespace mapping {


class Quartic : public Mapping {
public:
    // -- Exceptions
    // None

    // -- Contructors

    Quartic(const param::MIRParametrisation& parametrisation) : Mapping(parametrisation) {}

    // -- Destructor

    virtual ~Quartic();

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

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

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

    friend std::ostream& operator<<(std::ostream& s, const Quartic& p) {
        p.print(s);
        return s;
    }
};


}  // namespace mapping
}  // namespace transform
}  // namespace action
}  // namespace mir


#endif
