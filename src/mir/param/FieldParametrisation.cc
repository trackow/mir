/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/param/FieldParametrisation.h"

#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/config/MIRConfiguration.h"
#include "eckit/types/Fraction.h"
#include "mir/param/SimpleParametrisation.h"
#include "eckit/parser/YAMLParser.h"


namespace mir {
namespace param {


namespace {


inline double shift(const double& a, const double& b, double increment) {
    const eckit::Fraction inc(increment);
    eckit::Fraction shift = a - (a / inc).integralPart() * inc;

    if (!((a - b) / inc).integer()) {
        std::ostringstream oss;
        oss << "Cannot compute shift with a=" << a << ", b=" << b << ", inc=" << double(inc)
            << " shift=" << double(shift) << " (a-b)/inc=" << double((a - b) / inc);
        throw eckit::SeriousBug(oss.str());
    }

    return shift;
}
}  // (anonymous namespace)




static pthread_once_t once = PTHREAD_ONCE_INIT;
static std::map<long, SimpleParametrisation*> parameters_;


static void init() {


    eckit::Value c = eckit::YAMLParser::decodeFile("~mir/etc/mir/classes.yaml");

    // c.dump(std::cout) << std::endl;

    eckit::ValueMap classes = c;

    std::map<std::string, SimpleParametrisation*> p;

    for (auto i = classes.begin(); i != classes.end(); ++i) {
        const std::string& klass = (*i).first;
        eckit::ValueMap values = (*i).second;

        SimpleParametrisation* s = new SimpleParametrisation();

        for (auto j = values.begin(); j != values.end(); ++j) {

            std::string name = (*j).first;
            eckit::Value value = (*j).second;

            s->set(name, std::string(value));
        }

        p[klass] = s;
    }


    eckit::ValueMap parameters = eckit::YAMLParser::decodeFile("~mir/etc/mir/parameters.yaml");
    for (auto i = parameters.begin(); i != parameters.end(); ++i) {
        const std::string& klass = (*i).first;

        auto j = p.find(klass);
        if (j == p.end()) {
            std::ostringstream oss;
            oss << "Unknown class [" << klass << "]";
            eckit::SeriousBug(oss.str());
        }

        eckit::ValueList l = (*i).second;
        // std::cout << l << std::endl;

        for (auto p = l.begin(); p != l.end(); ++p) {

            long paramId = *p;

            auto k = parameters_.find(paramId);
            if (k != parameters_.end()) {
                std::ostringstream oss;
                oss << "More than one class defined for paramId=" << paramId;
                eckit::SeriousBug(oss.str());
            }

            parameters_[paramId] = (*j).second;
        }
    }

}



FieldParametrisation::FieldParametrisation() {}


FieldParametrisation::~FieldParametrisation() {}


bool FieldParametrisation::has(const std::string& name) const {

    // FIXME: not very elegant
    // if (name == "spectral") {
    //     long dummy;
    //     return get("truncation", dummy);
    // }

    eckit::Log::debug<LibMir>() << "FieldParametrisation::has(" << name << ") "
                                << *this << std::endl;
    return false;
}


bool FieldParametrisation::get(const std::string& name, std::string& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, bool& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, int& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, long& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, float& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, double& value) const {
    double inc;
    double a;
    double b;

    if (name == "west_east_shift") {
        if (get("west_east_increment", inc) && get("east", a) && get("west", b)) {
            value = shift(a, b, inc);
            return true;
        }
    }

    if (name == "south_north_shift") {
        if (get("south_north_increment", inc) && get("north", a) && get("south", b)) {
            value = shift(a, b, inc);
            return true;
        }
    }

    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, std::vector<int>& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, std::vector<long>& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, std::vector<float>& value) const {
    return _get(name, value);
}


bool FieldParametrisation::get(const std::string& name, std::vector<double>& value) const {

    // Check if this is in the MIRConfiguration
    if (_get(name, value)) {
        return true;
    }

    // Special cases

    if (name == "grid") {
        std::vector<double> grid(2, 0.);

        if (get("west_east_increment", grid[0]) && get("south_north_increment", grid[1])) {
            value.swap(grid);
            return true;
        }
    }

    if (name == "area") {
        std::vector<double> area(4, 0.);

        if (get("north", area[0]) && get("west", area[1]) && get("south", area[2]) && get("east", area[3])) {
            value.swap(area);
            return true;
        }
    }

    if (name == "latitudes") {
        latitudes(value);
        return true;
    }

    if (name == "longitudes") {
        longitudes(value);
        return true;
    }

    return false;
}


bool FieldParametrisation::get(const std::string& name, std::vector<std::string>& value) const {
    return _get(name, value);
}


template <class T>
bool FieldParametrisation::_get(const std::string& name, T& value) const {

    ASSERT(name != "paramId");

    // This assumes that other input (NetCDF, etc) also return a paramId

    long paramId;
    if (!get("paramId", paramId)) {
        return false;
    }

    // return paramId specific parametrisation

    pthread_once(&once, init);

    const auto j = parameters_.find(paramId);

    if (j == parameters_.end()) {
        std::ostringstream oss;
        oss << "No interpolation information for paramId=" << paramId;
        eckit::SeriousBug(oss.str());
    }

    return (*j).second->get(name, value);
}


void FieldParametrisation::latitudes(std::vector<double>&) const {
    std::ostringstream os;
    os << "FieldParametrisation::latitudes() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void FieldParametrisation::longitudes(std::vector<double>&) const {
    std::ostringstream os;
    os << "FieldParametrisation::longitudes() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


}  // namespace param
}  // namespace mir
