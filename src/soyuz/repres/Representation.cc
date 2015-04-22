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


#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"

#include "soyuz/param/MIRParametrisation.h"

#include "soyuz/repres/Representation.h"


namespace mir {
namespace repres {
namespace {


static eckit::Mutex *local_mutex = 0;
static std::map<std::string, RepresentationFactory *> *m = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, RepresentationFactory *>();
}


}  // (anonymous namespace)


Representation::Representation() {
}


Representation::~Representation() {
}


void Representation::fill(grib_info &) const {
    eckit::StrStream os;
    os << "Representation::fill() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}


Representation *Representation::crop(double north, double west, double south, double east,
                                     const std::vector<double> &, std::vector<double> &) const {
    eckit::StrStream os;
    os << "Representation::crop() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}


Representation *Representation::truncate(size_t truncation,
        const std::vector<double> &, std::vector<double> &) const {
    eckit::StrStream os;
    os << "Representation::truncate() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}


size_t Representation::frame(std::vector<double> &values, size_t size, double missingValue) const {
    eckit::StrStream os;
    os << "Representation::frame() not implemented for " << *this << eckit::StrStream::ends;
    throw eckit::SeriousBug(std::string(os));
}


RepresentationFactory::RepresentationFactory(const std::string &name):
    name_(name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


RepresentationFactory::~RepresentationFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);

}


Representation *RepresentationFactory::build(const param::MIRParametrisation &params) {

    pthread_once(&once, init);


    std::string name;

    if (!params.get("gridType", name)) {
        throw eckit::SeriousBug("RepresentationFactory cannot get gridType");
    }

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, RepresentationFactory *>::const_iterator j = m->find(name);

    eckit::Log::info() << "Looking for RepresentationFactory [" << name << "]" << std::endl;

    if (j == m->end()) {
        eckit::Log::error() << "No RepresentationFactory for [" << name << "]" << std::endl;
        eckit::Log::error() << "RepresentationFactories are:" << std::endl;
        for (j = m->begin() ; j != m->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << std::endl;
        throw eckit::SeriousBug(std::string("No RepresentationFactory called ") + name);
    }

    return (*j).second->make(params);
}


}  // namespace repres
}  // namespace mir

