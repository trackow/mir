/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/gridbox/GridBoxMaximum.h"

#include <ostream>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"

#include "mir/method/WeightMatrix.h"
#include "mir/method/nonlinear/NonLinear.h"


namespace mir {
namespace method {
namespace gridbox {


struct NonLinearGridBoxMaximum : nonlinear::NonLinear {
    using NonLinear::NonLinear;
    bool treatment(Matrix& /*A*/, WeightMatrix& /*W*/, Matrix& /*B*/, const data::MIRValuesVector& /*values*/,
                   const double& /*missingValue*/) const {
        // TODO
        NOTIMP;
    }

private:
    bool sameAs(const NonLinear& other) const { return dynamic_cast<const GridBoxMaximum*>(&other); }
    void print(std::ostream& out) const { out << "GridBoxMaximum[]"; }
    void hash(eckit::MD5& h) const {
        std::ostringstream s;
        s << *this;
        h.add(s.str());
    }
};


GridBoxMaximum::GridBoxMaximum(const param::MIRParametrisation& param) : GridBoxMethod(param) {
    // TODO: MethodWeighted::pushNonLinear(new GridBoxMaximum(param));  (currently MethodWeighted::missing_)
}


const char* GridBoxMaximum::name() const {
    return "grid-box-maximum";
}


static MethodBuilder<GridBoxMaximum> __builder("grid-box-maximum");


}  // namespace gridbox
}  // namespace method
}  // namespace mir
