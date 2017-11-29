/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_action_interpolate_Gridded2RotatedReducedGGPLGiven_h
#define mir_action_interpolate_Gridded2RotatedReducedGGPLGiven_h

#include <vector>
#include "mir/action/interpolate/Gridded2GriddedInterpolation.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace action {
namespace interpolate {


class Gridded2RotatedReducedGGPLGiven : public Gridded2GriddedInterpolation {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Gridded2RotatedReducedGGPLGiven(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Gridded2RotatedReducedGGPLGiven(); // Change to virtual if base class

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

    // -- Members

    std::vector<long> pl_;
    util::Rotation rotation_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual bool sameAs(const Action& other) const;

    // From Gridded2GriddedInterpolation
    virtual const repres::Representation* outputRepresentation() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace interpolate
}  // namespace action
}  // namespace mir


#endif

