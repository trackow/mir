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


#ifndef LandSeaMask_H
#define LandSeaMask_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>


namespace mir {

namespace action {
class Action;
class ActionPlan;
}

namespace param {
class MIRParametrisation;
class RuntimeParametrisation;
}

namespace lsm {


class LandSeaMask {
  public:

// -- Exceptions
    // None

// -- Contructors

    LandSeaMask(const param::MIRParametrisation& parametrisation);

// -- Destructor

    virtual ~LandSeaMask(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods

    virtual void prepare(action::ActionPlan &) const = 0;

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  protected:

// -- Members

    const param::MIRParametrisation& parametrisation_;

// -- Methods


    virtual void print(std::ostream&) const = 0; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    LandSeaMask(const LandSeaMask&);
    LandSeaMask& operator=(const LandSeaMask&);

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

    friend std::ostream& operator<<(std::ostream& s,const LandSeaMask& p) {
        p.print(s);
        return s;
    }

};


class LandSeaMaskFactory {
    std::string name_;
    virtual LandSeaMask* make(const param::MIRParametrisation&) = 0 ;

  protected:

    LandSeaMaskFactory(const std::string&);
    virtual ~LandSeaMaskFactory();

  public:
    static LandSeaMask* build(const param::MIRParametrisation&);

};


template<class T>
class LandSeaMaskBuilder : public LandSeaMaskFactory {
    virtual LandSeaMask* make(const param::MIRParametrisation& param) {
        return new T(param);
    }
  public:
    LandSeaMaskBuilder(const std::string& name) : LandSeaMaskFactory(name) {}
};


}  // namespace logic
}  // namespace mir
#endif

