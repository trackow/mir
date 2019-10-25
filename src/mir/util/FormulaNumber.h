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
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   April 2016


#ifndef mir_util_FormulaNumber_h
#define mir_util_FormulaNumber_h

#include "mir/util/Formula.h"


namespace mir {
namespace util {


class FormulaNumber : public Formula {
public:

    FormulaNumber(const param::MIRParametrisation &parametrisation, double value);
    virtual ~FormulaNumber();

private:

    virtual void print(std::ostream& s) const;
    virtual void execute(context::Context & ctx) const;
    virtual bool sameAs(const Action& other) const;
    virtual const char* name() const;

private: // members

    double value_;

};


}  // namespace util
}  // namespace mir


#endif
