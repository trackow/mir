/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Feb 2017


#ifndef mir_action_transform_VodSh2uvGridded_h
#define mir_action_transform_VodSh2uvGridded_h

#include "mir/action/transform/Sh2Gridded.h"


namespace mir {
namespace action {
namespace transform {


class VodSh2uvGridded : public Sh2Gridded {
public:

    // -- Exceptions
    // None

    // -- Contructors
    VodSh2uvGridded(const param::MIRParametrisation&);

    // -- Destructor
    virtual ~VodSh2uvGridded();

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
    void sh2grid(struct Trans_t&, data::MIRField&) const;

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
    // None

};


}  // namespace transform
}  // namespace action
}  // namespace mir


#endif
