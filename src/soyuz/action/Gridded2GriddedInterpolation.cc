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

#include "soyuz/action/Gridded2GriddedInterpolation.h"

#include <memory>

#include "atlas/GridSpec.h"

#include "soyuz/data/MIRField.h"
#include "soyuz/method/Method.h"
#include "soyuz/param/MIRParametrisation.h"
#include "soyuz/repres/Representation.h"



namespace mir {
namespace action {


Gridded2GriddedInterpolation::Gridded2GriddedInterpolation(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {
}


Gridded2GriddedInterpolation::~Gridded2GriddedInterpolation() {
}


void Gridded2GriddedInterpolation::execute(data::MIRField &field) const {
    //    NOTIMP;

    std::string name = "method.finite-element";

    // Here we need some ugly logic again
    if (0) {
        std::string param;
        ASSERT(parametrisation_.get("param", param));
        if (param == "large_scale_precipitation") { // This should be a lookup in a config file somewhere
            name = "method.mass-conserving";
        }
    }

    std::auto_ptr< method::Method > method(method::MethodFactory::build(name, parametrisation_));
    eckit::Log::info() << "method is " << *method << std::endl;

    const repres::Representation *in = field.representation();
    repres::Representation *out = outputRepresentation(field.representation());

    try {
        // TODO: We should not copy those things around

        atlas::GridSpec inspec  = in->gridSpec();
        atlas::GridSpec outspec = out->gridSpec();

        eckit::Log::info() << "ingrid  = " << inspec  << std::endl;
        eckit::Log::info() << "outgrid = " << outspec << std::endl;

        method->execute(field, inspec, outspec);

    } catch (...) {
        delete out;
        throw;
    }

    field.representation(out);

}


}  // namespace action
}  // namespace mir

