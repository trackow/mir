/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#include "mir/netcdf/Dimension.h"

#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/Dataset.h"
#include "mir/netcdf/Remapping.h"

#include <iostream>

namespace mir {
namespace netcdf {

Dimension::Dimension(Dataset &owner, const std::string &name, size_t len):
    owner_(owner),
    name_(name),
    len_(len),
    remapping_(new Remapping())
{
    remapping_->attach();
}

Dimension::~Dimension()
{
    if (remapping_) {
        remapping_->detach();
    }
}

void Dimension::dump(std::ostream &out) const
{
    out << "\t" << name_ << " = " << len_ << " ;" << std::endl;
}

bool Dimension::sameAs(const Dimension &other) const {
    return name_ == other.name_;
}

bool Dimension::inUse() const {
    std::vector<Variable *> v = owner_.variablesForDimension(*this);
    return v.size() != 0;
}

const std::string &Dimension::name() const {
    return name_;
}

size_t Dimension::count() const {
    return len_;
}

void Dimension::grow(size_t)
{
    std::cout << __FUNCTION__ << " " << *this << std::endl;
    NOTIMP;
}

void Dimension::clone(Dataset &) const {
    std::cout << __FUNCTION__ << " " << *this << std::endl;
    NOTIMP;
}

void Dimension::create(int) const {
    std::cout << __FUNCTION__ << " " << *this << std::endl;
    NOTIMP;
}

int Dimension::id()const {
    std::cout << __FUNCTION__ << " " << *this << std::endl;
    NOTIMP;
    return 0;
}

void Dimension::remapping(Remapping *remapping) {
    if (remapping) {
        remapping->attach();
    }
    if (remapping_) {
        remapping_->detach();
    }
    remapping_ = remapping;
}

const Remapping &Dimension::remapping() const {
    ASSERT(remapping_ != 0);
    return *remapping_;
}

void Dimension::realDimensions(std::vector<size_t>& dims) const {
    std::ostringstream os;
    os << "Dimension::realDimensions() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

}
}
