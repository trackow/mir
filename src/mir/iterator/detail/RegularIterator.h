/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_iterator_detail_RegularIterator_h
#define mir_iterator_detail_RegularIterator_h

#include "eckit/types/Fraction.h"


namespace mir {
namespace iterator {
namespace detail {


class RegularIterator {
public:

    // -- Exceptions
    // None

    // -- Constructors

    RegularIterator(const eckit::Fraction& a, const eckit::Fraction& b, const eckit::Fraction& inc, const eckit::Fraction& ref);

    RegularIterator(const eckit::Fraction& a, const eckit::Fraction& b, const eckit::Fraction& inc, const eckit::Fraction& ref, const eckit::Fraction& period);

    // -- Destructor

    virtual ~RegularIterator() = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static eckit::Fraction adjust(const eckit::Fraction& target, const eckit::Fraction& inc, bool up);

    const eckit::Fraction& a() const {
        return a_;
    }

    const eckit::Fraction& b() const {
        return b_;
    }

    const eckit::Fraction& inc() const {
        return inc_;
    }

    size_t n() const {
        return n_;
    }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    eckit::Fraction a_;
    eckit::Fraction b_;
    eckit::Fraction inc_;
    size_t n_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace detail
}  // namespace iterator
}  // namespace mir


#endif

