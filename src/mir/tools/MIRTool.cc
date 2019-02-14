/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Sep 2016


#include "mir/tools/MIRTool.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/system/Library.h"

#include "mir/api/mir_config.h"
#include "mir/api/Atlas.h"

#if defined(HAVE_ECCODES)
#include "eccodes.h"
#endif


namespace mir {
namespace tools {


static MIRTool* instance_ = nullptr;


static void usage(const std::string& tool) {
    ASSERT(instance_);
    instance_->usage(tool);
}


MIRTool::MIRTool(int argc, char** argv) : eckit::Tool(argc, argv, "MIR_HOME") {
    ASSERT(instance_ == nullptr);
    instance_ = this;
    options_.push_back(new eckit::option::SimpleOption<bool>("version", "Display the version number"));
}


void MIRTool::run() {

    eckit::option::CmdArgs args(&mir::tools::usage,
                                options_,
                                numberOfPositionalArguments(),
                                minimumPositionalArguments());

    if (args.has("version")) {
        auto& log = eckit::Log::info();

        using eckit::system::Library;
        for (const auto& lib_name : Library::list()) {
            auto& lib = Library::lookup(lib_name);
            log << lib.name()
                << " " << lib.version()
                << " git-sha1:" << lib.gitsha1(8)
                << " home:" << lib.libraryHome()
                << std::endl;
        }

#if ATLAS_HAVE_TRANS
        log << "transi " << transi_version() << " git-sha1:" << transi_git_sha1_abbrev(8) << std::endl;
        log << "trans " << trans_version() << " git-sha1:" << trans_git_sha1_abbrev(8) << std::endl;
#endif

#if defined(HAVE_ECCODES)
        log << "eccodes " << ECCODES_VERSION_STR  << " git-sha1:" << std::string(codes_get_git_sha1()).substr(0,8) << std::endl;
#endif
    }

    init(args);
    execute(args);
    finish(args);
}


void MIRTool::init(const eckit::option::CmdArgs& args) {
#if defined(HAVE_ATLAS)
    atlas::Library::instance().initialise(args);
#endif
}


void MIRTool::finish(const eckit::option::CmdArgs&) {
#if defined(HAVE_ATLAS)
    atlas::Library::instance().finalise();
#endif
}


}  // namespace tools
}  // namespace mir
