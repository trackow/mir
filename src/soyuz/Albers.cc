
// File Albers.cc
// Baudouin Raoult - (c) ECMWF Apr 15

#include "Albers.h"
#include "MIRParametrisation.h"
#include "eckit/exception/Exceptions.h"

#include <iostream>

Albers::Albers(const MIRParametrisation &parametrisation) {
}

Albers::Albers()
{
}


Albers::~Albers() {
}


void Albers::print(std::ostream &out) const {
    out << "Albers["
        << "]";
}


void Albers::fill(grib_spec &spec) const  {
    NOTIMP;
}



static RepresentationBuilder<Albers> albers("albers"); // Name is what is returned by grib_api
