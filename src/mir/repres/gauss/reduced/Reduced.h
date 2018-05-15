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


#ifndef Reduced_H
#define Reduced_H

#include "mir/repres/gauss/Gaussian.h"


namespace mir {
namespace util {
class Rotation;
}
}


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


class Reduced : public Gaussian {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Reduced(const param::MIRParametrisation&);
    Reduced(size_t N, const util::BoundingBox& = util::BoundingBox());

    // -- Destructor

    virtual ~Reduced();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    size_t k_;
    size_t Nj_;

    // -- Methods

    Iterator* unrotatedIterator() const;
    Iterator* rotatedIterator(const util::Rotation&) const;

    void setNj();

    // -- Overridden methods

    virtual void fill(grib_info&) const;
    virtual void fill(api::MIRJob&) const;
    virtual bool sameAs(const Representation&) const;
    virtual void correctWestEast(Longitude& w, Longitude& e, bool grib1 = false) const;

    // -- Class members

    // -- Class methods
    // None

private:

    // -- Members
    // None

    // -- Methods

    virtual const std::vector<long>& pls() const = 0;

    // -- Overridden methods

    virtual size_t frame(std::vector<double>& values, size_t size, double missingValue) const;
    virtual size_t numberOfPoints() const;
    virtual bool getLongestElementDiagonal(double&) const;
    eckit::Fraction getSmallestIncrement() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const Reduced& p)
    //  { p.print(s); return s; }

};


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir


#endif

