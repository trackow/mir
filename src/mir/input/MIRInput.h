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


#ifndef mir_input_MIRInput_h
#define mir_input_MIRInput_h

#include <iosfwd>


// Forward declaration only, not need for grib_api
typedef struct grib_handle grib_handle;


namespace mir {
namespace data {
class MIRField;
}
namespace param {
class MIRParametrisation;
}
}


namespace mir {
namespace input {


class MIRInput {

protected:

    MIRInput();

public:

    virtual ~MIRInput();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual bool next();
    virtual size_t dimensions() const;

    virtual const param::MIRParametrisation& parametrisation(size_t which = 0) const = 0;

    virtual data::MIRField field() const = 0;
    virtual grib_handle* gribHandle(size_t which = 0) const;

    virtual size_t copy(double* values, size_t size) const;

    virtual bool sameAs(const MIRInput& other) const = 0;

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

    virtual void print(std::ostream&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    MIRInput(const MIRInput&);
    MIRInput& operator=(const MIRInput&);

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

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s,const MIRInput& p) {
        p.print(s);
        return s;
    }

};


}  // namespace input
}  // namespace mir


#endif

