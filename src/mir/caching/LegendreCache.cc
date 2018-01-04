/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "mir/caching/LegendreCache.h"

#include "mir/config/LibMir.h"

using namespace eckit;

namespace mir {
namespace caching {

//----------------------------------------------------------------------------------------------------------------------

LegendreCache::LegendreCache() :
    CacheManager<LegendreCacheTraits>("LegendreCache", // dummy -- would be used in load() / save() static functions
                                      LibMir::cacheDir(),
                                      eckit::Resource<bool>("$MIR_THROW_ON_CACHE_MISS;mirThrowOnCacheMiss",
                                                            false), 0) {
}

const char *LegendreCacheTraits::name() {
    return "mir/coeffs";
}

int LegendreCacheTraits::version() {
    return 1;
}

const char *LegendreCacheTraits::extension() {
    return ".leg";
}

void LegendreCacheTraits::save(const eckit::CacheManagerBase&, LegendreCacheTraits::value_type&, const PathName&) {
}

void LegendreCacheTraits::load(const eckit::CacheManagerBase&, LegendreCacheTraits::value_type &, const PathName&) {
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace method
}  // namespace mir

