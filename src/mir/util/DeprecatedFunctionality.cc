/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date October 2017


#include "mir/util/DeprecatedFunctionality.h"

#include <set>
#include "eckit/utils/MD5.h"


namespace mir {
namespace util {


namespace {
static std::set<eckit::Hash::digest_t> known_messages;
}  // (anonymous namespace)


DeprecatedFunctionality::DeprecatedFunctionality(const std::string& msg, std::ostream& out) {

    // only log DeprecatedFunctionality messages once
    const eckit::Hash::digest_t digest = eckit::MD5(msg).digest();
    if (known_messages.insert(digest).second) {
        message(msg, out);
    }
}


void DeprecatedFunctionality::message(const std::string& msg, std::ostream& out) {
    out << "DeprecatedFunctionality: " << msg << std::endl;
}


}  // namespace util
}  // namespace mir
