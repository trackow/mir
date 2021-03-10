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


#include "mir/key/packing/Packing.h"

#include <map>
#include <mutex>
#include <ostream>
#include <set>

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Grib.h"
#include "mir/util/Log.h"


namespace mir {
namespace key {
namespace packing {


static std::once_flag once;
static std::mutex* local_mutex                    = nullptr;
static std::map<std::string, PackingFactory*>* ms = nullptr;
static std::map<std::string, PackingFactory*>* mg = nullptr;
static void init() {
    local_mutex = new std::mutex();
    ms          = new std::map<std::string, PackingFactory*>();
    mg          = new std::map<std::string, PackingFactory*>();
}


Packing::Packing(const std::string& name, const param::MIRParametrisation& param) :
    defineAccuracy_(false),
    defineEdition_(false),
    definePacking_(false),
    gridded_(param.userParametrisation().has("grid") || param.fieldParametrisation().has("gridded")) {
    auto& user  = param.userParametrisation();
    auto& field = param.fieldParametrisation();

    if (user.get("accuracy", accuracy_)) {
        long accuracy;
        defineAccuracy_ = !field.get("accuracy", accuracy) || accuracy_ != accuracy;
    }

    if (user.get("edition", edition_)) {
        long edition;
        defineEdition_ = !field.get("edition", edition) || edition_ != edition;
    }

    ASSERT(!name.empty());
    packing_ = name;
    std::string packing;
    definePacking_ = !field.get("packing", packing) || packing_ != packing;
}


Packing::~Packing() = default;


bool Packing::sameAs(Packing* other) const {
    if (definePacking_ != other->definePacking_ || defineAccuracy_ != other->defineAccuracy_ ||
        defineEdition_ != other->defineEdition_) {
        return false;
    }
    return (definePacking_ && packing_ == other->packing_) && (defineAccuracy_ && accuracy_ == other->accuracy_) &&
           (defineEdition_ && edition_ == other->edition_);
}


bool Packing::printParametrisation(std::ostream& out) const {
    std::string sep;

    if (definePacking_) {
        out << sep << "packing=" << packing_;
        sep = ",";
    }

    if (defineEdition_) {
        out << sep << "edition=" << edition_;
        sep = ",";
    }

    if (defineAccuracy_) {
        out << sep << "accuracy=" << accuracy_;
        sep = ",";
    }

    return !sep.empty();
}


bool Packing::empty() const {
    return !definePacking_ && !defineAccuracy_ && !defineEdition_;
}


void Packing::requireEdition(const param::MIRParametrisation& param, long edition) {
    // Define edition if not specified
    if (defineEdition_) {
        ASSERT(edition_ == edition);
        return;
    }

    if (!param.fieldParametrisation().get("edition", edition_) || edition_ != edition) {
        edition_       = edition;
        defineEdition_ = true;
    }
}


void Packing::fill(grib_info& info, long pack) const {
    info.packing.packing       = CODES_UTIL_PACKING_SAME_AS_INPUT;
    info.packing.accuracy      = CODES_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT;
    info.packing.editionNumber = 0;

    if (definePacking_) {
        info.packing.packing      = CODES_UTIL_PACKING_USE_PROVIDED;
        info.packing.packing_type = pack;
    }

    if (defineAccuracy_) {
        info.packing.accuracy     = CODES_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES;
        info.packing.bitsPerValue = accuracy_;
    }

    if (defineEdition_) {
        info.packing.editionNumber = edition_;
    }
}


void Packing::set(grib_handle* h, const std::string& type) const {
    // Note: order is important

    if (defineAccuracy_) {
        GRIB_CALL(codes_set_long(h, "bitsPerValue", accuracy_));
    }

    if (defineEdition_) {
        GRIB_CALL(codes_set_long(h, "edition", edition_));
    }

    if (definePacking_) {
        auto len = type.length();
        GRIB_CALL(codes_set_string(h, "packingType", type.c_str(), &len));
    }
}


PackingFactory::PackingFactory(const std::string& name, const std::string& alias, bool spectral, bool gridded) :
    name_(name), spectral_(spectral), gridded_(gridded) {
    std::call_once(once, init);
    std::lock_guard<std::mutex> lock(*local_mutex);

    ASSERT(gridded || spectral);

    if (gridded) {
        ASSERT_MSG(mg->insert({name, this}).second, "PackingFactory: duplicate gridded packing '" + name + "'");
        if (!alias.empty()) {
            ASSERT_MSG(mg->insert({alias, this}).second, "PackingFactory: duplicate gridded packing '" + name + "'");
        }
    }

    if (spectral) {
        ASSERT_MSG(ms->insert({name, this}).second, "PackingFactory: duplicate spectral packing '" + name + "'");
        if (!alias.empty()) {
            ASSERT_MSG(ms->insert({alias, this}).second, "PackingFactory: duplicate spectral packing '" + name + "'");
        }
    }
}


PackingFactory::~PackingFactory() {
    std::lock_guard<std::mutex> lock(*local_mutex);

    mg->erase(name_);
    ms->erase(name_);
}


Packing* PackingFactory::build(const param::MIRParametrisation& param) {
    std::call_once(once, init);
    std::lock_guard<std::mutex> lock(*local_mutex);

    auto& user  = param.userParametrisation();
    auto& field = param.fieldParametrisation();


    // When converting from spectral to gridded, default to simple packing
    std::string name = user.has("grid") && field.has("spectral") ? "simple" : "av";
    user.get("packing", name);


    std::string packing;
    field.get("packing", packing);


    bool av       = name == "av" || name == "archived-value";
    bool gridded  = user.has("grid") || field.has("gridded");
    std::string t = gridded ? "gridded" : "spectral";
    const auto& m = gridded ? *mg : *ms;


    // In case of packing=av, try instantiating specific packing
    if (av) {
        Log::debug() << "PackingFactory: looking for '" << packing << "'" << std::endl;

        auto j = m.find(packing);
        if (j != m.end()) {
            return j->second->make(packing, param);
        }
    }

    Log::debug() << "PackingFactory: looking for '" << name << "'" << std::endl;
    auto j = m.find(name);
    if (j != m.end()) {
        return j->second->make(av ? packing : j->second->name_, param);
    }

    Log::error() << "PackingFactory: unknown " << t << " packing '" << name << "', choices are: ";
    for (const auto& j : m) {
        Log::error() << ", " << j.first;
    }

    throw exception::SeriousBug("PackingFactory: unknown " + t + " packing '" + name + "'");
}


void PackingFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::mutex> lock(*local_mutex);

    std::set<std::string> p;
    for (const auto& j : *ms) {
        p.insert(j.first);
    }
    for (const auto& j : *mg) {
        p.insert(j.first);
    }

    const char* sep = "";
    for (const auto& j : p) {
        out << sep << j;
        sep = ", ";
    }
}


}  // namespace packing
}  // namespace key
}  // namespace mir
