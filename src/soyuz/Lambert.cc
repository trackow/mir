
// File Lambert.cc
// Baudouin Raoult - (c) ECMWF Apr 15

#include "Lambert.h"
#include "MIRParametrisation.h"
#include "eckit/exception/Exceptions.h"

#include <iostream>

Lambert::Lambert(const MIRParametrisation &parametrisation) {
}

Lambert::Lambert()
{
}


Lambert::~Lambert() {
}


void Lambert::print(std::ostream &out) const {
    out << "Lambert["
        << "]";
}


void Lambert::fill(grib_spec &spec) const  {
    NOTIMP;
}



static RepresentationBuilder<Lambert> lambert("lambert"); // Name is what is returned by grib_api
