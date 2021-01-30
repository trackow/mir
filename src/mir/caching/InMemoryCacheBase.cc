/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/caching/InMemoryCacheBase.h"

#include "eckit/config/Resource.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace caching {


static eckit::Mutex* local_mutex       = nullptr;
static std::set<InMemoryCacheBase*>* m = nullptr;
static pthread_once_t once             = PTHREAD_ONCE_INIT;


static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::set<InMemoryCacheBase*>();
}


InMemoryCacheBase::InMemoryCacheBase() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    ASSERT(m->find(this) == m->end());
    m->insert(this);
}

InMemoryCacheBase::~InMemoryCacheBase() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    ASSERT(m->find(this) != m->end());
    m->erase(this);
}

InMemoryCacheUsage InMemoryCacheBase::totalFootprint() {

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    InMemoryCacheUsage result;

    for (auto& j : *m) {
        result += j->footprint();
    }

    return result;
}

void InMemoryCacheBase::checkTotalFootprint() {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    constexpr size_t CAPACITY_MEMORY = 1024LL * 1024 * 1024 * 1024 * 1024 * 1024;
    constexpr size_t CAPACITY_SHARED = CAPACITY_MEMORY;
    static eckit::Resource<InMemoryCacheUsage> totalInMemoryCacheCapacity(
        "mirTotalInMemoryCacheCapacity;$MIR_TOTAL_CACHE_MEMORY_FOOTPRINT",
        InMemoryCacheUsage(CAPACITY_MEMORY, CAPACITY_SHARED));

    InMemoryCacheUsage maximumCapacity = totalInMemoryCacheCapacity;

    if (!maximumCapacity && m->empty()) {
        return;
    }


    bool more = true;
    while (more) {

        more = false;

        InMemoryCacheUsage totalFootprint;

        for (auto& j : *m) {
            totalFootprint += j->footprint();
        }

        Log::debug() << "CACHE-checkTotalFootprint size " << totalFootprint << ", max is " << maximumCapacity
                     << std::endl;

        if (totalFootprint > maximumCapacity) {

            InMemoryCacheUsage p = (totalFootprint - maximumCapacity) / m->size();


            for (auto& j : *m) {
                InMemoryCacheUsage purged = j->purge(p);
                if (purged) {
                    Log::debug() << "CACHE-checkTotalFootprint purged " << purged << " from " << j->name() << std::endl;
                    more = true;
                }
            }
        }
    }
}


}  // namespace caching
}  // namespace mir
