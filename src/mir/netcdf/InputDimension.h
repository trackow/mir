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


#ifndef mir_netcdf_InputDimension_h
#define mir_netcdf_InputDimension_h

#include "mir/netcdf/Dimension.h"


namespace mir {
namespace netcdf {


class InputDimension : public Dimension {
    InputDimension(const InputDimension&) = delete;
    InputDimension& operator=(const InputDimension&) = delete;

public:
    InputDimension(Dataset& owner, const std::string& name, int id, size_t len);
    virtual ~InputDimension() override;

private:
    // Members

    int id_;

    // -- Methods

    // From Dimension
    virtual void print(std::ostream&) const override;
    virtual void clone(Dataset& owner) const override;
    virtual int id() const override;
    virtual void realDimensions(std::vector<size_t>& dims) const override;
};


}  // namespace netcdf
}  // namespace mir


#endif
