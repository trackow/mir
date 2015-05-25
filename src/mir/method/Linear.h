/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#ifndef mir_method_Bilinear_H
#define mir_method_Bilinear_H

#include "mir/method/FiniteElement.h"


namespace mir {
namespace method {


class Linear: public FiniteElement {
  public:

    Linear(const param::MIRParametrisation&);

    virtual ~Linear();

  protected:

    virtual void hash( eckit::MD5& ) const;

  private:

// -- Methods
    // None

// -- Overridden methods

    virtual void generateMesh(const atlas::Grid& g, atlas::Mesh& mesh) const;
    virtual void print(std::ostream&) const;
    virtual const char* name() const;

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

};


}  // namespace method
}  // namespace mir

#endif

