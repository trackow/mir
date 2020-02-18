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


#include "mir/packing/Complex.h"
#include "mir/repres/Representation.h"
#include "mir/util/Grib.h"

#include <iostream>

namespace mir {
namespace packing {


Complex::Complex(const std::string& name) : Packer(name) {}


Complex::~Complex() = default;

void Complex::print(std::ostream& out) const {
    out << "Complex[]";
}

void Complex::fill(grib_info& info, const repres::Representation& repres) const {
    info.packing.packing = CODES_UTIL_PACKING_USE_PROVIDED;
    repres.setComplexPacking(info);
}


static Complex packing1("complex");
static Complex packing2("co");


}  // namespace packing
}  // namespace mir
