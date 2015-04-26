/*
 * (C) Copyright 1996-2015 ECMWF.
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


#include "eckit/runtime/Tool.h"

#include "mir/api/MIRJob.h"
#include "mir/input/GribFileInput.h"
#include "mir/output/GribFileOutput.h"


class MIRDtest : public eckit::Tool {

    virtual void run();

  public:
    MIRDtest(int argc, char **argv) :
        eckit::Tool(argc, argv) {
    }
};


void MIRDtest::run() {
    mir::api::MIRJob job;

    job.set("grid", 10, 10);

    mir::input::GribFileInput input("reduced_gg.grib");
    mir::output::GribFileOutput output("reduced_gg_to_regular_gg.grib");

    while (input.next()) {
        job.execute(input, output);
    }
}


int main(int argc, char **argv) {
    MIRDtest tool(argc, argv);
    tool.start();
    return 0;
}

