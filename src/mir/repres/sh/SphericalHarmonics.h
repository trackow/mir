/*
 * (C) Copyright 1996-2016 ECMWF.
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


#ifndef mir_repres_sh_SphericalHarmonics_h
#define mir_repres_sh_SphericalHarmonics_h

#include "mir/repres/Representation.h"
#include "mir/util/Domain.h"


namespace mir {
namespace repres {
namespace sh {


class SphericalHarmonics : public Representation {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    SphericalHarmonics(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~SphericalHarmonics();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    util::Domain domain() const {
        return util::Domain::makeGlobal();
    }

    // -- Class members
    // None

    // -- Class methods

    static void truncate(size_t truncation_from, size_t truncation_to, const std::vector<double>& in, std::vector<double>& out);

    static size_t number_of_complex_coefficients(size_t truncation) {
        return (truncation + 1) * (truncation + 2) / 2;
    }

  protected:

    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    SphericalHarmonics(size_t truncation);

    // No copy allowed

    SphericalHarmonics(const SphericalHarmonics&);
    SphericalHarmonics& operator=(const SphericalHarmonics&);

    // -- Members

    size_t truncation_;

    // Number of values (complex packing unpacked subset) for which scaling and packing are not applied
    size_t Ts_;

    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info&) const;
    virtual void fill(api::MIRJob &) const;
    virtual const Representation* truncate(size_t truncation,
                                           const std::vector<double>&, std::vector<double>&) const;
    virtual size_t truncation() const;
    virtual size_t pentagonalResolutionTs() const;

    virtual void comparison(std::string&) const;

    virtual void validate(const std::vector<double>&) const;

    virtual void setComplexPacking(grib_info&) const;
    virtual void setSimplePacking(grib_info&) const;
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation& other) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const SphericalHarmonics& p)
    //  { p.print(s); return s; }

};


}  // namespace sh
}  // namespace repres
}  // namespace mir


#endif

