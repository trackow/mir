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


#ifndef mir_netcdf_MergeCoordinateStep_h
#define mir_netcdf_MergeCoordinateStep_h

#include "mir/netcdf/Step.h"


namespace mir {
namespace netcdf {
class Variable;
}
}  // namespace mir


namespace mir {
namespace netcdf {


class MergeCoordinateStep : public Step {
public:
    MergeCoordinateStep(Variable& out, const Variable& in);
    ~MergeCoordinateStep();

private:
    // Members

    Variable& out_;
    const Variable& in_;

    // -- Methods
    virtual void print(std::ostream& out) const override;
    virtual int rank() const override;
    virtual void execute(MergePlan& plan) override;
};


}  // namespace netcdf
}  // namespace mir


#endif
