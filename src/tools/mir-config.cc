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


#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/utils/StringTools.h"

#include "mir/input/GribFileInput.h"
#include "mir/param/CombinedParametrisation.h"
#include "mir/param/DefaultParametrisation.h"
#include "mir/param/FieldParametrisation.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/Log.h"


using namespace mir;


struct Param {
    Param(const std::vector<std::string>& classes_v) : classes(classes_v.begin(), classes_v.end()) {}
    long id;
    std::string name;
    std::set<std::string> classes;
};


struct Map : std::map<long, std::string> {
    Map& move_or_remove(Param& p) {
        if (!name.empty() && p.classes.find(name) != p.classes.end()) {
            operator[](p.id) = p.name;
        }
        else {
            erase(p.id);
        }
        p.classes.erase(name);

        return *this;
    }

    void reset(const std::string& _name) {
        ASSERT(!_name.empty());
        name = _name;
        clear();
    }

    friend std::ostream& operator<<(std::ostream& out, const Map& m) {
        out << m.name << ":\n";
        for (auto& p : m) {
            p.second.empty() ? (out << "- " << p.first << "\n")
                             : (out << "- " << p.first << "  # " << p.second << "\n");
        }
        return out;
    }

    std::string name;
};


struct MIRConfig : tools::MIRTool {
    MIRConfig(int argc, char** argv) : MIRTool(argc, argv) {
        using eckit::option::SimpleOption;

        options_.push_back(new SimpleOption<long>("param-id", "Display configuration with paramId"));
        options_.push_back(new SimpleOption<std::string>("key", "Display configuration with specific key"));

        options_.push_back(new SimpleOption<std::string>("param-class", "Set class(es) for paramId, /-separated"));
        options_.push_back(new SimpleOption<std::string>("param-name", "Set name for paramId"));
    }

    int minimumPositionalArguments() const override { return 0; }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool
                    << " [--key=key] [[--param-id=value]|[input1.grib [input2.grib [...]]]]"
                       "\n"
                       "Examples: "
                       "\n"
                       "  % "
                    << tool
                    << ""
                       "\n"
                       "  % "
                    << tool
                    << " --param-id=157"
                       "\n"
                       "  % "
                    << tool << " --key=lsm input1.grib input2.grib" << std::endl;
    }

    void execute(const eckit::option::CmdArgs&) override;

    void display(const param::MIRParametrisation& metadata, const std::string& key) const {
        static param::SimpleParametrisation empty;
        static param::DefaultParametrisation defaults;
        const param::CombinedParametrisation combined(empty, metadata, defaults);
        const param::MIRParametrisation& param(combined);

        long paramId = 0;
        ASSERT(metadata.get("paramId", paramId));

        std::string value = "???";
        param.get(key, value);

        Log::info() << "paramId=" << paramId << ": " << key << "=" << value << std::endl;
    }
};


void MIRConfig::execute(const eckit::option::CmdArgs& args) {

    std::string key("interpolation");
    args.get("key", key);

    std::string klass;
    if (args.get("param-class", klass)) {
        Param param(eckit::StringTools::split("/", klass));
        ASSERT(args.get("param-id", param.id));
        args.get("param-name", param.name);

        Map map;

        eckit::LocalPathName to("parameter-class.yaml");
        eckit::LocalPathName to_tmp(to + ".tmp");
        auto from(to.exists() ? to : "~mir/etc/mir/parameter-class.yaml");

        // update (temporary) file
        {
            std::ifstream i(std::string(from).c_str());
            ASSERT(i);

            std::ofstream o(to_tmp.c_str());
            ASSERT(o);
            o << "---\n\n";

            for (std::string line; std::getline(i, line);) {
                if (!line.empty() && std::isalpha(line[0]) != 0) {
                    if (!map.name.empty()) {
                        o << map.move_or_remove(param) << "\n";
                    }

                    map.reset(line.substr(0, line.find_first_of(':')));
                    continue;
                }

                if (!line.empty() && line.substr(0, 2) == "- ") {
                    auto c = line.find_first_of('#');
                    long id;
                    std::istringstream(line.substr(2, c)) >> id;
                    std::string name(c != std::string::npos ? eckit::StringTools::trim(line.substr(c + 1)) : "");

                    map[id] = name;
                }
            }

            if (!map.name.empty()) {
                o << map.move_or_remove(param) << "\n";
            }

            for (auto& name : param.classes) {
                map.reset(name);
                o << map.move_or_remove(param) << "\n";
            }
        }

        // rename updated file
        eckit::LocalPathName::rename(to_tmp, to.fullName());
        return;
    }


    // Display configuration for a paramId
    long paramId = 0;
    if (args.get("param-id", paramId) || args.count() == 0) {

        class DummyField : public param::FieldParametrisation {
            long paramId_;
            void print(std::ostream&) const override {}
            bool get(const std::string& name, long& value) const override {
                if (name == "paramId") {
                    value = paramId_;
                    return true;
                }
                return FieldParametrisation::get(name, value);
            }

        public:
            DummyField(long paramId) : paramId_(paramId) {}
        };

        display(DummyField(paramId), key);
    }
    else {

        for (size_t i = 0; i < args.count(); i++) {

            // Display configuration for each input file message(s)
            input::GribFileInput grib(args(i));
            while (grib.next()) {
                input::MIRInput& input = grib;
                display(input.parametrisation(), key);
            }
        }
    }
}


int main(int argc, char** argv) {
    MIRConfig tool(argc, argv);
    return tool.start();
}
