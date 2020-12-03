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


#ifndef mir_action_filter_AddRandomFilter_h
#define mir_action_filter_AddRandomFilter_h

#include "mir/action/plan/Action.h"

#include <memory>


namespace mir {
namespace stats {
class Distribution;
}
}  // namespace mir


namespace mir {
namespace action {
namespace filter {


class AddRandomFilter : public Action {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    AddRandomFilter(const param::MIRParametrisation&);
    AddRandomFilter(const AddRandomFilter&) = delete;

    // -- Destructor

    virtual ~AddRandomFilter() = default;

    // -- Convertors
    // None

    // -- Operators

    AddRandomFilter& operator=(const AddRandomFilter&) = delete;

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
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::unique_ptr<stats::Distribution> distribution_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Action
    virtual bool sameAs(const Action&) const override;
    virtual const char* name() const override;
    void print(std::ostream&) const override;
    virtual void execute(context::Context&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace filter
}  // namespace action
}  // namespace mir


#endif
