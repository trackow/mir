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


#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"
#include "eckit/exception/Exceptions.h"

#include "mir/log/MIR.h"
#include "mir/action/plan/Action.h"


namespace mir {
namespace action {

namespace {


static eckit::Mutex *local_mutex = 0;
static std::map<std::string, ActionFactory *> *m = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<std::string, ActionFactory *>();
}


}  // (anonymous namespace)


Action::Action(const param::MIRParametrisation &parametrisation):
    parametrisation_(parametrisation) {
}


Action::~Action() {
}


ActionFactory::ActionFactory(const std::string &name):
    name_(name) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if(m->find(name) != m->end()) {
        throw eckit::SeriousBug("ActionFactory: duplication action: " + name);
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


ActionFactory::~ActionFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);

}


Action *ActionFactory::build(const std::string &name, const param::MIRParametrisation &params) {

    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, ActionFactory *>::const_iterator j = m->find(name);

    eckit::Log::trace<MIR>() << "Looking for ActionFactory [" << name << "]" << eckit::newl;

    if (j == m->end()) {
        eckit::Log::error() << "No ActionFactory for [" << name << "]" << eckit::newl;
        eckit::Log::error() << "ActionFactories are:" << eckit::newl;
        for (j = m->begin() ; j != m->end() ; ++j)
            eckit::Log::error() << "   " << (*j).first << eckit::newl;
        throw eckit::SeriousBug(std::string("No ActionFactory called ") + name);
    }

    return (*j).second->make(params);
}

}  // namespace action
}  // namespace mir

