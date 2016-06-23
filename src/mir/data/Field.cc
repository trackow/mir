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


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/Field.h"
#include "mir/repres/Representation.h"
#include "mir/data/MIRFieldStats.h"


namespace mir {
namespace data {


Field::Field(const param::MIRParametrisation &param, bool hasMissing, double missingValue):
    values_(),
    hasMissing_(hasMissing),
    missingValue_(missingValue),
    representation_(repres::RepresentationFactory::build(param)) {

    if (representation_) {
        representation_->attach();
    }
}


Field::Field(const repres::Representation *repres, bool hasMissing, double missingValue):
    values_(),
    hasMissing_(hasMissing),
    missingValue_(missingValue),
    representation_(repres) {

    if (representation_) {
        representation_->attach();
    }
}

Field::Field(const Field& other):
    values_(other.values_),
    paramId_(other.paramId_),
    hasMissing_(other.hasMissing_),
    missingValue_(other.missingValue_),
    representation_(other.representation_)
{

    if (representation_) {
        representation_->attach();
    }
}

Field *Field::clone() const {
    return new Field(*this);
}

// Warning: take ownership of values
void Field::update(std::vector<double> &values, size_t which) {
    if (values_.size() <= which) {
        values_.resize(which + 1);
    }
    std::swap(values_[which], values);
}

size_t Field::dimensions() const {
    return values_.size();
}

void Field::dimensions(size_t size)  {
    return values_.resize(size);
}

Field::~Field() {
    if (representation_) {
        representation_->detach();
    }
}

void Field::print(std::ostream &out) const {

    out << "Field[count="<< count() << ",";
    out << "dimensions=" << values_.size();
    if (hasMissing_) {
        out << ",missingValue=" << missingValue_;
    }

    if (representation_) {
        out << ",representation=" << *representation_;
    }

    if (paramId_.size()) {
        out << ",params=";
        char sep = '(';
        for (size_t i = 0; i < paramId_.size(); i++) {
            out << sep << paramId_[i];
            sep = ',';
        }
        out << ')';
    }

    out << "]";
}


const repres::Representation *Field::representation() const {
    ASSERT(representation_);
    return representation_;
}

void Field::validate() const {

    if (representation_) {
        for (size_t i = 0; i < values_.size(); i++) {
            representation_->validate(values(i));
        }
    }
}

MIRFieldStats Field::statistics(size_t i) const {

    if (hasMissing_) {
        const std::vector<double> &vals = values(i);
        std::vector<double> tmp;
        tmp.reserve(vals.size());
        size_t missing = 0;

        for (size_t j = 0; j < vals.size(); j++) {
            if (vals[j] != missingValue_) {
                tmp.push_back(vals[j]);
            } else {
                missing++;
            }
        }
        return MIRFieldStats(tmp, missing);
    }
    return MIRFieldStats(values(i), 0);
}

void Field::representation(const repres::Representation *representation) {
    if (representation) {
        representation->attach();
    }
    if (representation_) {
        representation_->detach();
    }
    representation_ = representation;
}

const std::vector<double> &Field::values(size_t which) const {
    ASSERT(which < values_.size());
    return values_[which];
}

std::vector<double> &Field::direct(size_t which)  {
    ASSERT(which < values_.size());
    return values_[which];
}

void Field::paramId(size_t which, size_t param) {
    while (paramId_.size() <= which) {
        paramId_.push_back(0);
    }
    paramId_[which] = param;
}

size_t Field::paramId(size_t which) const {

    if (paramId_.size() <= which) {
        return 0;
    }

    return paramId_[which];
}

bool Field::hasMissing() const {
    return hasMissing_;
}

double Field::missingValue() const {
    return missingValue_;
}


void Field::hasMissing(bool on) {
    hasMissing_ = on;
}

void Field::missingValue(double value)  {
    missingValue_ = value;
}

}  // namespace data
}  // namespace mir

