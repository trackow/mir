/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/gauss/reduced/ReducedClassic.h"

#include <iostream>

#include "mir/repres/gauss/reduced/ReducedFromPL.h"

namespace mir {
namespace repres {
namespace reduced {


ReducedClassic::ReducedClassic(size_t N):
    Classic(N) {

}

ReducedClassic::~ReducedClassic() {
}

void ReducedClassic::print(std::ostream &out) const {
    out << "ReducedGGClassic[N" << N_ << ",bbox=" << bbox_ << "]";
}


Reduced *ReducedClassic::cropped(const util::BoundingBox &bbox, const std::vector<long> &pl) const {
    // We lose the ReducedClassic nature of the grid
    return new ReducedFromPL(N_, pl, bbox);
}


}  // namespace reduced
}  // namespace repres
}  // namespace mir

