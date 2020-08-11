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


#ifndef mir_key_grid_Grid_h
#define mir_key_grid_Grid_h

#include <iosfwd>
#include <string>


namespace mir {
namespace param {
class MIRParametrisation;
class SimpleParametrisation;
}  // namespace param
namespace repres {
class Representation;
}
namespace util {
class Rotation;
}
}  // namespace mir


namespace mir {
namespace key {
namespace grid {


class Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Grid(const Grid&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    Grid& operator=(const Grid&) = delete;

    // -- Methods

    virtual const repres::Representation* representation() const;
    virtual const repres::Representation* representation(const util::Rotation&) const;
    virtual const repres::Representation* representation(const param::MIRParametrisation&) const;
    virtual void parametrisation(const std::string& grid, param::SimpleParametrisation&) const;
    virtual size_t gaussianNumber() const;

    static const Grid& lookup(const std::string& key);
    static bool known(const std::string& key);
    static void list(std::ostream&);

    bool isNamed() const { return gridType_ == named_t; }
    bool isTyped() const { return gridType_ == typed_t; }
    bool isRegularLL() const { return gridType_ == regular_ll_t; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Types

    enum grid_t
    {
        named_t,
        typed_t,
        regular_ll_t
    };

    // -- Constructors

    Grid(const std::string& key, grid_t);

    // -- Destructor

    virtual ~Grid();

    // -- Members

    std::string key_;

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    grid_t gridType_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Grid& p) {
        p.print(s);
        return s;
    }
};


}  // namespace grid
}  // namespace key
}  // namespace mir


#endif
