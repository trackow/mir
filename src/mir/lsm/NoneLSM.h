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


#ifndef NoneLSM_H
#define NoneLSM_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "mir/lsm/LSMChooser.h"

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
class RuntimeParametrisation;
}

namespace lsm {


class NoneLSM : public LSMChooser {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    NoneLSM(const std::string &name);

    // -- Destructor

    virtual ~NoneLSM(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static Mask& instance();


  protected:

    // -- Members

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

    NoneLSM(const NoneLSM &);
    NoneLSM &operator=(const NoneLSM &);

    // -- Members
    // None

    // -- Methods


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    virtual Mask *create(const std::string &, const std::string &,
                                const param::MIRParametrisation &param, const atlas::Grid &grid) const ;



    // -- Friends



};


}  // namespace logic
}  // namespace mir
#endif

