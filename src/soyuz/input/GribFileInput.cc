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


#include "eckit/io/BufferedHandle.h"

#include "soyuz/input/GribFileInput.h"


namespace mir {
namespace input {


GribFileInput::GribFileInput(const eckit::PathName &path):
    path_(path), handle_(0) {
}


GribFileInput::~GribFileInput() {
    if (handle_) {
        handle_->close();
        delete handle_;
    }
}


void GribFileInput::print(std::ostream &out) const {
    out << "GribFileInput[path=" << path_ << "]";
}


eckit::DataHandle &GribFileInput::dataHandle() {
    if (!handle_) {
        handle_ = new eckit::BufferedHandle(path_.fileHandle());
        handle_->openForRead();
    }
    return *handle_;
}


}  // namespace input
}  // namespace mir

