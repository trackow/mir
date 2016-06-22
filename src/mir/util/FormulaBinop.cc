/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   FormulaBinop.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   April 2016

#include <iostream>
#include <functional>
#include <cmath>


#include "mir/util/FormulaBinop.h"
#include "mir/util/Function.h"
#include "eckit/exception/Exceptions.h"
#include "mir/action/context/Context.h"

#include "eckit/exception/Exceptions.h"
#include "mir/action/context/Context.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"
#include "mir/data/MIRField.h"


namespace mir {
namespace util {

//----------------------------------------------------------------------------------------------------------------------

FormulaBinop::FormulaBinop(const param::MIRParametrisation &parametrisation,
                           const std::string& name,
                           Formula* arg1,
                           Formula *arg2):
    FormulaFunction(parametrisation, name, arg1, arg2) {

}

FormulaBinop::~FormulaBinop() {
}

void FormulaBinop::print(std::ostream& out) const {
    out << '(' << *args_[0] << ") " << function_ << " (" << *args_[1] << ')' ;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class Unop : public Function {

    T op_;

    virtual void print(std::ostream& s) const {
        s << name_;
    }

    void field(context::Context&) const {

        // eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().calcTiming_);
        // data::MIRField& field = ctx.field();

        // for (size_t j = 0; j < field.dimensions(); j++) {
        //     std::vector<double> &values = field.direct(j);

        //     size_t size = values.size();

        //     if (field.hasMissing()) {
        //         double missingValue = field.missingValue();

        //         for (size_t i = 0; i < size; i++) {
        //             if (values[i] != missingValue) {
        //                 values[i] = op_(values[i]);
        //             }
        //         }

        //     } else {
        //         for (size_t i = 0; i < size; i++) {
        //             values[i] = op_(values[i]);
        //         }
        //     }

        //     // field.paramId(j, param_);

        // }
    }

    void scalar(context::Context& a) const {
        a.scalar(op_(a.scalar()));
    }

    virtual void execute(context::Context& ctx) const {

        context::Context a = ctx.pop();

        if (a.isField()) {
            return field(a);
        }

        if (a.isScalar()) {
            return scalar(a);
        }

        std::ostringstream oss;
        oss << "Cannot compute " << *this << " with " << a;
        throw new eckit::SeriousBug(oss.str());

    }


public:
    Unop(const char* name) : Function(name) {}
};


template<typename T>
class Binop : public Function {

    T op_;

    virtual void print(std::ostream& s) const {
        s << name_;
    }

    void fieldField(context::Context& ctx, context::Context& ctx1, context::Context& ctx2) const {

        eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().calcTiming_);

        data::MIRField& field = ctx.field();

        data::MIRField& field1 = ctx1.field();
        data::MIRField& field2 = ctx2.field();

        ASSERT(field1.dimensions() == field2.dimensions());
        field.dimensions(field1.dimensions());

        for (size_t i = 0; i < field1.dimensions(); ++i)
        {


            const std::vector<double> &values1 = field1.values(i);
            const std::vector<double> &values2 = field2.values(i);

            size_t size = values1.size();
            ASSERT(values1.size() == values2.size());

            std::vector<double> values(size);


            // if (field1.hasMissing() || field2.hasMissing()) {
            //     double missingValue1 = field1.missingValue();
            //     double missingValue2 = field2.missingValue();

            //     for (size_t i = 0; i < size; i++) {
            //         if (values1[i] == missingValue || values2[i] == missingValue) {
            //             values1[i] = missingValue;
            //         } else {
            //             values1[i] = op_(values1[i], values2[i]);
            //         }
            //     }

            // } else
            {
                for (size_t i = 0; i < size; i++) {
                    values[i] = op_(values1[i], values2[i]);
                }
            }

            field.update(values, i);

        }
        // field.paramId(0, param_);
    }

    void scalarScalar(context::Context& ctx, context::Context& ctx1, context::Context& ctx2) const {
        ctx.scalar(op_(ctx1.scalar(), ctx2.scalar()));

    }

    void fieldScalar(context::Context& ctx, context::Context& ctx1, context::Context& ctx2) const {
        // eckit::AutoTiming timing(ctx1.statistics().timer_, ctx1.statistics().calcTiming_);

        // data::MIRField& field1 = ctx1.field();
        // double scalar2 = ctx2.scalar();

        // for (size_t i = 0; i < field1.dimensions(); ++i)
        // {

        //     std::vector<double> &values1 = field.direct(i);
        //     const std::vector<double> &values2 = field.values(i);

        //     size_t size = values1.size();
        //     ASSERT(values1.size() == values2.size());

        //     if (field.hasMissing()) {
        //         double missingValue = field.missingValue();

        //         for (size_t i = 0; i < size; i++) {
        //             if (values1[i] == missingValue || values2[i] == missingValue) {
        //                 values1[i] = missingValue;
        //             } else {
        //                 values1[i] = op_(values1[i], values2[i]);
        //             }
        //         }

        //     } else {
        //         for (size_t i = 0; i < size; i++) {
        //             values1[i] = op_(values1[i], values2[i]);
        //         }
        //     }


        // }
    }

    void scalarField(context::Context& ctx, context::Context& ctx1, context::Context& ctx2) const {

        // eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().calcTiming_);
        // data::MIRField& field = ctx.field();

        // ASSERT(field.dimensions() == 2);

        // std::vector<double> &values0 = field.direct(0);
        // const std::vector<double> &values1 = field.values(1);

        // size_t size = values0.size();
        // ASSERT(values0.size() == values1.size());

        // if (field.hasMissing()) {
        //     double missingValue = field.missingValue();

        //     for (size_t i = 0; i < size; i++) {
        //         if (values0[i] == missingValue || values1[i] == missingValue) {
        //             values0[i] = missingValue;
        //         } else {
        //             values0[i] = op_(values0[i], values1[i]);
        //         }
        //     }

        // } else {
        //     for (size_t i = 0; i < size; i++) {
        //         values0[i] = op_(values0[i], values1[i]);
        //     }
        // }

        // field.dimensions(1);
        // // field.paramId(0, param_);
    }

    virtual void execute(context::Context& ctx) const {
        // ASSERT(ctx.size() == 2);

        context::Context ctx2 = ctx.pop();
        context::Context ctx1 = ctx.pop();

        if (ctx1.isScalar() && ctx2.isScalar()) {
            return scalarScalar(ctx, ctx1, ctx2);
        }

        if (ctx1.isField() && ctx2.isField()) {
            return fieldField(ctx, ctx1, ctx2);
        }

        if (ctx1.isField() && ctx2.isScalar()) {
            return fieldScalar(ctx, ctx1, ctx2);
        }

        if (ctx1.isScalar() && ctx2.isField()) {
            return scalarField(ctx, ctx1, ctx2);
        }

        std::ostringstream oss;
        oss << "Cannot compute " << *this << " with " << ctx1 << " and " << ctx2;
        throw new eckit::SeriousBug(oss.str());

    }


public:
    Binop(const char* name) : Function(name) {}
};


static Binop<std::plus<double> > plus("+");
static Binop<std::minus<double> > minus("-");
static Binop<std::multiplies<double> > multiplies("*");
static Binop<std::divides<double> > divides("/");
// static Binop<std::modulus<double> > modulus1("%");
// static Binop<std::modulus<double> > modulus2("mod");

static Unop<std::negate<double> > negate("neg");


static Binop<std::equal_to<double> > equal_to("=");
static Binop<std::not_equal_to<double> > not_equal_to("!=");

static Binop<std::greater<double> > greater(">");
static Binop<std::less<double> > less("<");

static Binop<std::greater_equal<double> > greater_equal(">=");
static Binop<std::less_equal<double> > less_equal("<=");

static Binop<std::logical_and<double> > logical_and_1("&&");
static Binop<std::logical_or<double> > logical_or_1("||");

static Binop<std::logical_and<double> > logical_and_2("and");
static Binop<std::logical_or<double> > logical_or_2("or");


static Unop<std::logical_not<double> > logical_not("not");

//=========================================================
struct sqrt { double operator()(double x) const { return ::sqrt(x); }};
static Unop<sqrt> _sqrt("sqrt");

struct sin { double operator()(double x) const { return ::sin(x); }};
static Unop<sin> _sin("sin");

struct cos { double operator()(double x) const { return ::cos(x); }};
static Unop<cos> _cos("cos");

struct tan { double operator()(double x) const { return ::tan(x); }};
static Unop<tan> _tan("tan");

struct asin { double operator()(double x) const { return ::asin(x); }};
static Unop<asin> _asin("asin");

struct acos { double operator()(double x) const { return ::acos(x); }};
static Unop<acos> _acos("acos");

struct atan { double operator()(double x) const { return ::atan(x); }};
static Unop<atan> _atan("atan");

struct log { double operator()(double x) const { return ::log(x); }};
static Unop<log> _log("log");

struct log2 { double operator()(double x) const { return ::log2(x); }};
static Unop<log2> _log2("log2");

struct log10 { double operator()(double x) const { return ::log10(x); }};
static Unop<log10> _log10("log10");

struct exp { double operator()(double x) const { return ::exp(x); }};
static Unop<exp> _exp("exp");

struct abs { double operator()(double x) const { return ::fabs(x); }};
static Unop<abs> _abs("abs");

//=========================================================
struct atan2 { double operator()(double x, double y) const { return ::atan2(x, y); }};
static Binop<atan2> _atan2("atan2");
//=========================================================

struct pow { double operator()(double x, double y) const { return ::pow(x, y); }};
static Binop<pow> _pow_1("^");
static Binop<pow> _pow_2("pow");



} // namespace util
} // namespace mir
