/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#include "mir/method/fe/Tessellation.h"


namespace mir {
namespace method {
namespace fe {


Tessellation::Tessellation(const param::MIRParametrisation &param) :
    FiniteElement(param) {
    inputMeshGenerationParams_.meshGenerator_ = "delaunay";
}


Tessellation::~Tessellation() {
}

bool Tessellation::sameAs(const Method& other) const {
    const Tessellation* o = dynamic_cast<const Tessellation*>(&other);
    return o && FiniteElement::sameAs(other);
}


const char *Tessellation::name() const {
    return  "tesselation";
}


void Tessellation::hash( eckit::MD5 &md5) const {
    FiniteElement::hash(md5);
}


void Tessellation::print(std::ostream &out) const {
    out << "Tessellation[]";
}


namespace {
static MethodBuilder< Tessellation > __name("tessellation");
}


}  // namespace fe
}  // namespace method
}  // namespace mir

