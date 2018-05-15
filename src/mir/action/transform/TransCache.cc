/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Jan 2017


#include "mir/action/transform/TransCache.h"

#include "eckit/log/Log.h"
#include "eckit/exception/Exceptions.h"


namespace mir {
namespace action {
namespace transform {


TransCache::TransCache() :
    loader_(nullptr),
    inited_(false) {
}


TransCache::~TransCache() {
    if (inited_) {
        eckit::Log::info() << "Delete " << *this << std::endl;
#if 0
        trans_delete(&trans_);
#endif
    } else {
        eckit::Log::info() << "Not Deleting " << *this << std::endl;
    }
    delete loader_;
}


void TransCache::print(std::ostream& s) const {
    s << "TransCache[";
    if (loader_) {
        s << *loader_;
    }
    s << "]";
}


}  // namespace transform
}  // namespace action
}  // namespace mir
