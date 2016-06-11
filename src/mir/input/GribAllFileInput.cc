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

#include "mir/input/GribAllFileInput.h"
#include "mir/input/GribFileInput.h"

#include <iostream>

#include "mir/data/MIRField.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdFile.h"
#include "eckit/io/Buffer.h"
#include "mir/util/Grib.h"

namespace mir {
namespace input {


GribAllFileInput::GribAllFileInput(const std::string &path):
    path_(path),
    count_(0) {

    eckit::StdFile f(path);
    eckit::Buffer buffer(64 * 1024 * 1024);

    int e = 0;
    for (;;) {
        size_t len = buffer.size();
        off_t here = ftello(f);
        int e  = wmo_read_any_from_file(f, buffer, &len);
        if (e == GRIB_END_OF_FILE) {
            break;
        }

        if (e == GRIB_BUFFER_TOO_SMALL) {
            GRIB_ERROR(e, "wmo_read_any_from_file");
        }

        if (e != GRIB_SUCCESS) {
            GRIB_ERROR(e, "wmo_read_any_from_file");
        }

        inputs_.push_back(new GribFileInput(path, here));
    }
}


GribAllFileInput::~GribAllFileInput() {
    for (auto j = inputs_.begin(); j != inputs_.end(); ++j) {
        delete (*j);
    }
}


const param::MIRParametrisation &GribAllFileInput::parametrisation() const {
    // Assumes that all components have the same parametrisation
    ASSERT(inputs_.size());
    return inputs_[0]->parametrisation();
}

grib_handle *GribAllFileInput::gribHandle(size_t which) const {
    ASSERT(which < inputs_.size());
    return inputs_[which]->gribHandle();
}

data::MIRField *GribAllFileInput::field() const {
    // // Assumes that both component (e.g. U and V) have the same parametrisation
    // data::MIRField *u = component1_.field();
    // data::MIRField *v = component2_.field();

    // ASSERT(u->dimensions() == 1);
    // ASSERT(v->dimensions() == 1);

    // u->update(v->direct(0), 1);
    // delete v;

    // return u;
    NOTIMP;
}

bool GribAllFileInput::next() {
    if (count_ == 0) {
        for (auto j = inputs_.begin(); j != inputs_.end(); ++j) {
            // std::cout << *(*j) << std::endl;
            ASSERT((*j)->next());
        }
        return true;
    }
    return false;
}


bool GribAllFileInput::sameAs(const MIRInput &other) const {
    const GribAllFileInput *o = dynamic_cast<const GribAllFileInput *>(&other);
    return o && path_ == o->path_;
}


void GribAllFileInput::print(std::ostream &out) const {
    out << "GribAllFileInput[" << path_ << ",size=" << inputs_.size() << "]";
}

size_t GribAllFileInput::dimensions() const {
    return inputs_.size();
}

}  // namespace input
}  // namespace mir

