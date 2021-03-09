/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_key_packing_IEEE_h
#define mir_key_packing_IEEE_h

#include "mir/key/packing/Packing.h"


namespace mir {
namespace key {
namespace packing {


class IEEE : public Packing {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    IEEE(const param::MIRParametrisation&);

    // -- Destructor
    // None

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

    long precision_;
    long bitsPerValue_;

    // -- Methods
    // None

    // -- Overridden methods

    void fill(grib_info&) const override;
    void set(grib_handle*) const override;
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace packing
}  // namespace key
}  // namespace mir


#endif
