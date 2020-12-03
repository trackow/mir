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


#ifndef mir_action_transform_ShTruncate_h
#define mir_action_transform_ShTruncate_h

#include "mir/action/plan/Action.h"


namespace mir {
namespace action {
namespace transform {


class ShTruncate : public Action {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ShTruncate(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~ShTruncate() override;  // Change to virtual if base class

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

    void print(std::ostream&) const override;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t truncation_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual bool sameAs(const Action&) const override;

    virtual void execute(context::Context&) const override;

    virtual const char* name() const override;

    virtual void estimate(context::Context&, api::MIREstimation& estimation) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend ostream& operator<<(ostream& s,const Sh2ShTransform& p)
    //	{ p.print(s); return s; }
};


}  // namespace transform
}  // namespace action
}  // namespace mir


#endif
