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
/// @author Tiago Quintino
///
/// @date Oct 2016

#include "mir/caching/interpolator/FileLoader.h"

#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>

#include "eckit/io/StdFile.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"

#include "mir/config/LibMir.h"
#include "mir/method/WeightMatrix.h"

using mir::method::WeightMatrix;

namespace mir {
namespace caching {
namespace interpolator {


//----------------------------------------------------------------------------------------------------------------------


FileLoader::FileLoader(const param::MIRParametrisation& parametrisation, const eckit::PathName& path) :
    InterpolatorLoader(parametrisation, path),
    buffer_(path.size()) {

    /// FIXME buffer size is based on file.size() -- which is assumed to be bigger than the memory footprint

    eckit::Log::debug<LibMir>() << "Loading interpolator coefficients from " << path << std::endl;

    WeightMatrix w(path);

    w.dump(buffer_);
}

FileLoader::~FileLoader() {}

void FileLoader::print(std::ostream& out) const {
    out << "FileLoader[path=" << path_ << ",size=" << eckit::Bytes(buffer_.size()) << "]";
}

const void* FileLoader::address() const {
    return buffer_;
}

size_t FileLoader::size() const {
    return buffer_.size();
}

namespace {
static InterpolatorLoaderBuilder<FileLoader> loader("file-io");
}


//----------------------------------------------------------------------------------------------------------------------



} // namespace interpolator
} // namespace caching
} // namespace mir
