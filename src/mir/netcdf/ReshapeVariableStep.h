/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#ifndef mir_netcdf_ReshapeVariableStep
#define mir_netcdf_ReshapeVariableStep

#include "mir/netcdf/Step.h"

namespace mir {
namespace netcdf {

class Variable;
class Dimension;


class ReshapeVariableStep : public Step {
public:

    ReshapeVariableStep(Variable &out, const Dimension &dimension, size_t growth);
    virtual ~ReshapeVariableStep();

private:
    // Members

    Variable &out_;
    const Dimension &dimension_;
    size_t growth_;
    ReshapeVariableStep *next_;

    // -- Methods
    virtual void print(std::ostream &out) const;
    virtual int rank() const;
    virtual void execute(MergePlan &plan);
    virtual bool merge(Step *other);

};

}
}
#endif
