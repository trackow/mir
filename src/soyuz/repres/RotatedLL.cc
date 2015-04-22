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


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "soyuz/param/MIRParametrisation.h"

#include "soyuz/repres/RotatedLL.h"


namespace mir {
namespace repres {


RotatedLL::RotatedLL(const param::MIRParametrisation &parametrisation) {
}


RotatedLL::RotatedLL() {
}


RotatedLL::~RotatedLL() {
}


void RotatedLL::print(std::ostream &out) const {
    out << "RotatedLL["
        << "]";
}


void RotatedLL::fill(grib_info &info) const  {
    NOTIMP;
}


namespace {
static RepresentationBuilder<RotatedLL> rotatedLL("rotated_ll"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

