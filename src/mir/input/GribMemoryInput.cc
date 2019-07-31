/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/input/GribMemoryInput.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/util/Grib.h"


namespace mir {
namespace input {


GribMemoryInput::GribMemoryInput(const void* message, size_t length) {
    ASSERT(codes_check_message_header_footer(reinterpret_cast<const unsigned char*>(message), length, PRODUCT_GRIB) == 0);
    ASSERT(handle(grib_handle_new_from_message(nullptr, const_cast<void*>(message), length)));
}

GribMemoryInput::~GribMemoryInput() = default;

bool GribMemoryInput::sameAs(const MIRInput& other) const {
    return this == &other;
}

void GribMemoryInput::print(std::ostream& out) const {
    out << "GribMemoryInput[]";
}

}  // namespace input
}  // namespace mir

