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


#ifndef mir_netcdf_Step_h
#define mir_netcdf_Step_h

#include <iosfwd>


namespace mir {
namespace netcdf {
class MergePlan;
}
}  // namespace mir


namespace mir {
namespace netcdf {

class Step {
    Step(const Step&) = delete;
    Step& operator=(const Step&) = delete;

public:
    Step()          = default;
    virtual ~Step() = default;

    virtual int rank() const              = 0;
    virtual void execute(MergePlan& plan) = 0;
    virtual bool merge(Step* other);

private:
    virtual void print(std::ostream& s) const = 0;
    friend std::ostream& operator<<(std::ostream& s, const Step& v) {
        v.print(s);
        return s;
    }
};


struct CompareSteps {
    bool operator()(const Step* a, const Step* b) const { return a->rank() > b->rank(); }
};


}  // namespace netcdf
}  // namespace mir


#endif
