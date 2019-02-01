/*
 * (C) Copyright 1996- ECMWF.
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


#include "ActionPlan.h"

#include <fstream>

#include "eckit/exception/Exceptions.h"

#include "mir/action/plan/Action.h"
#include "mir/config/LibMir.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/action/context/Context.h"


namespace mir {
namespace action {


ActionPlan::ActionPlan(const param::MIRParametrisation &parametrisation):
    parametrisation_(parametrisation) {
}


ActionPlan::~ActionPlan() {
    for (auto& p : actions_) {
        delete p;
    }

    for (auto& p : runtimes_) {
        delete p;
    }
}


void ActionPlan::add(const std::string &name)  {
    ASSERT(!ended());
    actions_.push_back(ActionFactory::build(name, parametrisation_));
}


void ActionPlan::add(const std::string &name, const std::string &param, long value) {
    ASSERT(!ended());

    auto runtime = new param::RuntimeParametrisation(parametrisation_);
    runtimes_.push_back(runtime);
    runtime->set(param, value);
    actions_.push_back(ActionFactory::build(name, *runtime));
}


void ActionPlan::add(const std::string &name, const std::string &param, const std::string& value)  {
    ASSERT(!ended());

    auto runtime = new param::RuntimeParametrisation(parametrisation_);
    runtimes_.push_back(runtime);
    runtime->set(param, value);
    actions_.push_back(ActionFactory::build(name, *runtime));
}


void ActionPlan::add(const std::string &name, const std::string &param1, const std::string &value1, const std::string &param2, long value2) {
    ASSERT(!ended());

    auto runtime = new param::RuntimeParametrisation(parametrisation_);
    runtimes_.push_back(runtime);
    runtime->set(param1, value1);
    runtime->set(param2, value2);
    actions_.push_back(ActionFactory::build(name, *runtime));
}


void ActionPlan::add(const std::string &name, const std::string &param1,  const std::string &value1, const std::string &param2, const std::string &value2) {
    ASSERT(!ended());

    auto runtime = new param::RuntimeParametrisation(parametrisation_);
    runtimes_.push_back(runtime);
    runtime->set(param1, value1);
    runtime->set(param2, value2);
    actions_.push_back(ActionFactory::build(name, *runtime));
}


void ActionPlan::add(Action *action)  {
    ASSERT(!ended());

    actions_.push_back(action);
}


void ActionPlan::add(const std::string &name, param::MIRParametrisation* runtime) {
    ASSERT(!ended());

    ASSERT(runtime);
    runtimes_.push_back(runtime);
    actions_.push_back(ActionFactory::build(name, *runtime, false));
}


void ActionPlan::execute(context::Context & ctx) const {
    ASSERT(ended());

    std::string dumpPlanFile;
    parametrisation_.get("dump-plan-file", dumpPlanFile);

    if (dumpPlanFile.size()) {

        if (dumpPlanFile == "-") {
            custom(std::cout);
            std::cout << std::endl;
        }
        else {
            std::ofstream out(dumpPlanFile, std::ios::app);
            custom(out);
            out << std::endl;
        }
    }

    bool dryrun = false;
    if (parametrisation_.get("dryrun", dryrun) && dryrun) {
        return;
    }

    const char* sep = "###################################################################################";

    for (const auto& p : actions_) {
        eckit::Log::debug<LibMir>() << "Executing:"
                                    << "\n" << sep
                                    << "\n" << *p
                                    << "\n" << sep
                                    << std::endl;
        p->perform(ctx);
        eckit::Log::debug<LibMir>() << "Result:"
                                    << "\n" << sep
                                    << "\n" << ctx
                                    << "\n" << sep
                                    << std::endl;
    }
}


void ActionPlan::compress() {
    ASSERT(ended());
    eckit::Log::debug<LibMir>() << "ActionPlan::compress ===>" << std::endl;

    bool more = true;
    while (more) {
        more = false;

        for (size_t i = 0; i < actions_.size() - 1; ++i) {
            std::ostringstream oldAction;
            oldAction << *actions_[i];

            if (actions_[i]->mergeWithNext(*actions_[i + 1])) {

                eckit::Log::debug<LibMir>()
                        << "ActionPlan::compress: "
                        << "\n   " << oldAction.str()
                        << "\n + " << *actions_[i + 1]
                        << "\n = " << *actions_[i]
                        << std::endl;
                
                delete actions_[i + 1];
                actions_.erase(actions_.begin() + i + 1);

                more = true;
                break;
            }

            if (actions_[i]->deleteWithNext(*actions_[i + 1])) {

                eckit::Log::debug<LibMir>()
                        << "ActionPlan::compress: "
                        << "\n   " << oldAction.str()
                        << "\n + " << *actions_[i + 1]
                        << "\n = " << *actions_[i + 1]
                        << std::endl;

                delete actions_[i];
                actions_.erase(actions_.begin() + i);

                more = true;
                break;
            }
        }
    }

    eckit::Log::debug<LibMir>() << "ActionPlan::compress <===" << std::endl;
}


bool ActionPlan::empty() const {
    return actions_.empty();
}


bool ActionPlan::ended() const {
    if (empty()) {
        return false;
    }
    return actions_.back()->isEndAction();
}


size_t ActionPlan::size() const {
    return actions_.size();
}


const Action &ActionPlan::action(size_t n) const {
    ASSERT(n < actions_.size());
    return *actions_[n];
}


void ActionPlan::print(std::ostream &out) const {
    out << "ActionPlan[";
    const char *arrow = "";
    for (const auto& p : actions_) {
        out << arrow << *p;
        arrow = " ==> ";
    }
    out << "]";
}


void ActionPlan::dump(std::ostream &out) const {
    for (const auto& p : actions_) {
        out << "      ==> " << *p << std::endl;
    }
}


void ActionPlan::custom(std::ostream &out) const {
    const char *sep = "";
    for (const auto& p : actions_) {
        out << sep;
        p->custom(out);
        sep = "|";
    }
}


}  // namespace action
}  // namespace mir

