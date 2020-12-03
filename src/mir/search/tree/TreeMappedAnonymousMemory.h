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


#ifndef mir_search_tree_TreeMappedAnonymousMemory_h
#define mir_search_tree_TreeMappedAnonymousMemory_h

#include "mir/search/tree/TreeMapped.h"


namespace mir {
namespace search {
namespace tree {


class TreeMappedAnonymousMemory : public TreeMapped {

    virtual bool ready() const override { return false; }

    virtual void commit() override {}

    virtual void print(std::ostream& out) const override { out << "TreeMappedAnonymousMemory[]"; }

public:
    TreeMappedAnonymousMemory(const repres::Representation&, const param::MIRParametrisation&);
};


}  // namespace tree
}  // namespace search
}  // namespace mir


#endif
