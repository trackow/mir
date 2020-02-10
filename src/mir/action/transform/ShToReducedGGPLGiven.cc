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


#include "mir/action/transform/ShToReducedGGPLGiven.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/action/transform/InvtransScalar.h"
#include "mir/action/transform/InvtransVodTouv.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/ReducedFromPL.h"


namespace mir {
namespace action {
namespace transform {


template <class Invtrans>
ShToReducedGGPLGiven<Invtrans>::ShToReducedGGPLGiven(const param::MIRParametrisation& parametrisation) :
    ShToGridded(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("pl", pl_));
}


template <class Invtrans>
ShToReducedGGPLGiven<Invtrans>::~ShToReducedGGPLGiven() = default;


template <class Invtrans>
bool ShToReducedGGPLGiven<Invtrans>::sameAs(const Action& other) const {
    auto o = dynamic_cast<const ShToReducedGGPLGiven*>(&other);
    return o && (pl_ == o->pl_);
}


template <class Invtrans>
void ShToReducedGGPLGiven<Invtrans>::print(std::ostream& out) const {
    out << "ShToReducedGGPLGiven[";
    ShToGridded::print(out);
    out << ",";
    Invtrans::print(out);
    out << ",pl=" << pl_.size() << "]";
}


template <class Invtrans>
void ShToReducedGGPLGiven<Invtrans>::sh2grid(data::MIRField& field, const ShToGridded::atlas_trans_t& trans,
                                             const param::MIRParametrisation& parametrisation) const {
    Invtrans::sh2grid(field, trans, parametrisation);
}


template <class Invtrans>
const char* ShToReducedGGPLGiven<Invtrans>::name() const {
    return "ShToReducedGGPLGiven";
}


template <class Invtrans>
const repres::Representation* ShToReducedGGPLGiven<Invtrans>::outputRepresentation() const {
    size_t N = pl_.size() / 2;
    return new repres::gauss::reduced::ReducedFromPL(N, pl_);
}


static ActionBuilder<ShToReducedGGPLGiven<InvtransScalar> > __action1("transform.sh-scalar-to-reduced-gg-pl-given");
static ActionBuilder<ShToReducedGGPLGiven<InvtransVodTouv> > __action2("transform.sh-vod-to-uv-reduced-gg-pl-given");


}  // namespace transform
}  // namespace action
}  // namespace mir
