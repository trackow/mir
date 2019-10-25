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
/// @date Jul 2016


#include "mir/param/CachedParametrisation.h"

#include "eckit/types/Types.h"


namespace mir {
namespace param {

CachedParametrisation::CachedParametrisation(MIRParametrisation& parametrisation):
    parametrisation_(parametrisation) {
}

CachedParametrisation::~CachedParametrisation() = default;

void CachedParametrisation::print(std::ostream &out) const {
    out << "CachedParametrisation[" << parametrisation_ << "]";
}

template<class T>
bool CachedParametrisation::_get(const std::string& name, T& value) const {
    MIRParametrisation& cache = cache_;

    // std::cout << "Get (cache) " << parametrisation_ << " " << name << std::endl;;

    if (cache.get(name, value)) {
        return true;
    }

    // std::cout << "Get (next) " << parametrisation_ << " " << name << std::endl;;

    if (parametrisation_.get(name, value)) {
        _set(name, value);
        return true;
    }

    return false;
}


template<class T>
void CachedParametrisation::_set(const std::string& name, const T& value) const {
    // std::cout << "Set " << *this << " " << name << " " << value << std::endl;
    cache_.set(name, value);
}

bool CachedParametrisation::has(const std::string& name) const {
    const MIRParametrisation& cache = cache_;
    return cache.has(name) || parametrisation_.has(name);
}

bool CachedParametrisation::get(const std::string& name, std::string& value) const {
    return _get(name, value);

}
bool CachedParametrisation::get(const std::string& name, bool& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, int& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, long& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, float& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, double& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, std::vector<int>& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, std::vector<long>& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, std::vector<float>& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, std::vector<double>& value) const {
    return _get(name, value);
}

bool CachedParametrisation::get(const std::string& name, std::vector<std::string>& value) const {
    return _get(name, value);
}

void CachedParametrisation::reset() {
//    std::cout << "Reset " << *this << std::endl;
    cache_.reset();
}

void CachedParametrisation::set(const std::string& name, int value) {
    _set(name, value);
}

void CachedParametrisation::set(const std::string& name, long value) {
    _set(name, value);
}

void CachedParametrisation::set(const std::string& name, double value) {
    _set(name, value);
}

void CachedParametrisation::set(const std::string& name, const std::string& value) {
    _set(name, value);
}

void CachedParametrisation::set(const std::string& name, const char* value) {
    _set(name, std::string(value));
}




}  // namespace param
}  // namespace mir

