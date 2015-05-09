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


#ifndef LandSeaMasks_H
#define LandSeaMasks_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// #include "mir/data/MIRField.h"

namespace atlas {
class Grid;
}


namespace mir {

namespace data {
class MIRField;
}
namespace action {
class Action;
class ActionPlan;
}

namespace param {
class MIRParametrisation;
}

namespace lsm {

class Mask;

class LandSeaMasks {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    LandSeaMasks(const Mask &input, const Mask &output);

    // -- Destructor

    ~LandSeaMasks(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual bool active() const;
    virtual std::string uniqueID() const;
    virtual bool cacheable() const;

    virtual const data::MIRField &inputField() const;
    virtual const data::MIRField &outputField() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static  LandSeaMasks lookup(const param::MIRParametrisation &param, const atlas::Grid &in, const atlas::Grid &out);

  protected:

    // -- Members

    const Mask &input_;
    const Mask &output_;

    // -- Methods


    virtual void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // No copy allowed

    // LandSeaMasks(const LandSeaMasks &);
    // LandSeaMasks &operator=(const LandSeaMasks &);

    // -- Members
    // None

    // -- Methods


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods


    // -- Friends

    friend std::ostream &operator<<(std::ostream &s, const LandSeaMasks &p) {
        p.print(s);
        return s;
    }

};


}  // namespace logic
}  // namespace mir
#endif

