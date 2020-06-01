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


#include "mir/action/interpolate/Gridded2RegularGG.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/regular/RegularGG.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2RegularGG::Gridded2RegularGG(const param::MIRParametrisation& parametrisation) :
    Gridded2UnrotatedGrid(parametrisation), N_(0) {
    ASSERT(parametrisation_.userParametrisation().get("regular", N_));
    ASSERT(N_ > 0);
}


Gridded2RegularGG::~Gridded2RegularGG() = default;


bool Gridded2RegularGG::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2RegularGG*>(&other);
    return (o != nullptr) && (N_ == o->N_) && Gridded2GriddedInterpolation::sameAs(other);
}


void Gridded2RegularGG::print(std::ostream& out) const {
    out << "Gridded2RegularGG["
           "N="
        << N_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2RegularGG::outputRepresentation() const {
    return new repres::gauss::regular::RegularGG(N_);
}


const char* Gridded2RegularGG::name() const {
    return "Gridded2RegularGG";
}


static ActionBuilder<Gridded2RegularGG> grid2grid("interpolate.grid2regular-gg");


}  // namespace interpolate
}  // namespace action
}  // namespace mir
