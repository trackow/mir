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


#include "mir/input/GribFixes.h"

#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/utils/StringTools.h"

#include "mir/param/SimpleParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Mutex.h"
#include "mir/util/ValueMap.h"


namespace mir {
namespace input {


GribFixes::GribFixes() {
    readConfigurationFiles();
}


GribFixes::~GribFixes() {
    for (auto& rule : fixes_) {
        delete rule.first;
        delete rule.second;
    }
}


bool GribFixes::fix(const param::MIRParametrisation& input, param::SimpleParametrisation& fixed) {
    static util::recursive_mutex mtx;
    util::lock_guard<util::recursive_mutex> lock(mtx);

    for (auto& f : fixes_) {
        if ((f.first)->matches(input)) {
            ASSERT(f.second);
            Log::warning() << "GribFixes: applying fixes " << *(f.second) << std::endl;
            f.second->copyValuesTo(fixed);
            return true;
        }
    }

    return false;
}


void GribFixes::print(std::ostream& s) const {
    s << "GribFixes";
    eckit::JSON json(s);

    json.startObject();
    for (auto& fix : fixes_) {
        json << *(fix.first) << *(fix.second);
    }
    json.endObject();
}


void GribFixes::readConfigurationFiles() {
    static util::recursive_mutex mtx;
    util::lock_guard<util::recursive_mutex> lock(mtx);

    using eckit::StringTools;

    ASSERT(fixes_.empty());

    const eckit::PathName path = "~mir/etc/mir/GRIB.yaml";
    if (!path.exists()) {
        return;
    }

    eckit::ValueMap rules(eckit::YAMLParser::decodeFile(path));
    for (const auto& rule : rules) {

        // how the input is to be identified
        auto id = new param::SimpleParametrisation;
        ASSERT(id);

        for (auto& ids : StringTools::split(",", rule.first)) {
            auto keyValue = StringTools::split("=", ids);
            ASSERT(keyValue.size() == 2);

            auto key   = StringTools::trim(keyValue[0]);
            auto value = StringTools::trim(keyValue[1]);

            if (value.find('/') != std::string::npos) {
                auto values = StringTools::split("/", value);
                id->set(key, values);
            }
            else {
                id->set(key, value);
            }
        }

        // how the input is to be corrected
        auto fix = new param::SimpleParametrisation;
        ASSERT(fix);

        for (auto& fixes : static_cast<const eckit::ValueList&>(rule.second)) {
            util::ValueMap map(fixes);
            map.set(*fix);
        }

        fixes_.emplace_back(id, fix);
    }
}


}  // namespace input
}  // namespace mir
