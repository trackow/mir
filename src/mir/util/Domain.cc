/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor
 * does it submit to any jurisdiction.
 */


#include "mir/util/Domain.h"


namespace mir {
namespace util {


Domain::Domain(Latitude north, Longitude west, Latitude south, Longitude east) :
    BoundingBox(north, west, south, east) {
}


Domain Domain::makeGlobal() {
    return Domain(Latitude::NORTH_POLE, Longitude::GREENWICH,
                  Latitude::SOUTH_POLE, Longitude::GLOBE);
}

#ifdef HAVE_ATLAS
Domain::operator atlas::RectangularDomain() const {
    return atlas::RectangularDomain(
        {{west().value(),  east().value()} },
        {{south().value(), north().value()} } );
}
#endif


void Domain::print(std::ostream& os) const {
    os << "Domain["
       <<  "north=" << north()
        << ",west="  << west()
        << ",south=" << south()
        << ",east="  << east()
        << ",isGlobal=" << isGlobal()
       // << ",includesPoleNorth=" << includesPoleNorth()
       // << ",includesPoleSouth=" << includesPoleSouth()
       // << ",isPeriodicEastWest=" << isPeriodicEastWest()
       << "]";
}


} // namespace util
} // namespace mir
