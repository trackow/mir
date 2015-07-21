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

#include "mir/method/Conservative.h"

#include "eckit/log/Log.h"

#include "experimental/eckit/la/Vector.h"
#include "experimental/eckit/la/LinearAlgebra.h"

#include "atlas/Grid.h"
#include "atlas/Mesh.h"
#include "atlas/util/IndexView.h"
#include "atlas/actions/BuildXYZField.h"
#include "atlas/geometry/Triag3D.h"
#include "atlas/geometry/Quad3D.h"

#include "mir/param/MIRParametrisation.h"

using eckit::la::Vector;
using eckit::la::LinearAlgebra;
using atlas::Mesh;
using atlas::geometry::Triag3D;
using atlas::geometry::Quad3D;

namespace mir {
namespace method {

static const double oneThird  = 1./ 3.;
static const double oneFourth = 1./ 4.;

Conservative::Conservative(const param::MIRParametrisation &param) :
    FELinear(param) {
}

Conservative::~Conservative() {
}

void Conservative::computeLumpedMassMatrix(eckit::la::Vector& d, const atlas::Grid& g) const
{
    eckit::Log::info() << "Conservative::computeLumpedMassMatrix" << std::endl;

    Mesh& mesh = g.mesh();

    eckit::Log::info() << "Mesh " << mesh << std::endl;

    d.resize(g.npts());

    d.setZero();

    atlas::actions::BuildXYZField("xyz")(mesh); // ensure we have a 'xyz' field (output mesh may not have it)

    atlas::FunctionSpace& nodes  = mesh.function_space( "nodes" );
    atlas::ArrayView<double, 2> coords  ( nodes.field( "xyz" ));

    atlas::FunctionSpace& triags = mesh.function_space( "triags" );
    atlas::IndexView<int, 2> triag_nodes ( triags.field( "nodes" ) );

    atlas::FunctionSpace& quads = mesh.function_space( "quads" );
    atlas::IndexView<int, 2> quads_nodes ( quads.field( "nodes" ) );

// TODO we need to consider points that are virtual
//    size_t firstVirtualPoint = std::numeric_limits<size_t>::max();
//    if( nodes.metadata().has("NbRealPts") )
//        firstVirtualPoint = i_nodes.metadata().get<size_t>("NbRealPts");

/// TODO Must handle missing values

    size_t nb_triags = triags.metadata().has("nb_owned") ? triags.metadata().get<size_t>("nb_owned") : triags.shape(0);
    size_t nb_quads = quads.metadata().has("nb_owned") ? quads.metadata().get<size_t>("nb_owned") : quads.shape(0);

    // loop on all the elements

    for(size_t e = 0; e < nb_triags; ++e)
    {
        size_t idx [3];
        for(size_t n = 0; n<3; ++n)
          idx[n] = triag_nodes(e,n);

        Triag3D triag(coords[idx[0]].data(), coords[idx[1]].data(), coords[idx[2]].data());

        const double area = triag.area();

        /// TODO add check for virtuals

        for(size_t n = 0; n<3; ++n)
            d[ idx[n] ] += area * oneThird;
    }

    for(size_t e = 0; e < nb_quads; ++e)
    {
        size_t idx [4];
        for(size_t n = 0; n<4; ++n)
          idx[n] = quads_nodes(e,n);

        Quad3D quad(coords[idx[0]].data(), coords[idx[1]].data(), coords[idx[2]].data(), coords[idx[3]].data());

        const double area = quad.area();

        /// TODO add check for virtuals

        for(size_t n = 0; n<4; ++n)
            d[ idx[n] ] += area * oneFourth;
    }

}

void Conservative::assemble(WeightMatrix& W, const atlas::Grid& in, const atlas::Grid& out) const
{

    eckit::Log::info() << "Input  pts " << in.npts()
                       << "Output pts " << out.npts() << std::endl;

    // 1) IM_{ds} compute the interpolation matrix from destination (out) to source (input)

    WeightMatrix IM(in.npts(), out.npts());

    FELinear::assemble(IM, out, in);

    eckit::Log::info() << "IM rows " << IM.rows()
                       << " cols "   << IM.cols() << std::endl;

//    IM.save("IM.mat");

    // 2) M_s compute the lumped mass matrix of the source mesh

    generateMesh(in, in.mesh()); // input grid hasn't been tesselated mesh yet ...

    Vector M_s;
    computeLumpedMassMatrix(M_s, in);

    // 3) M_d^{-1} compute the inverse lumped mass matrix of the destination mesh

    Vector M_d;
    computeLumpedMassMatrix(M_d, out);

    for(size_t i = 0; i < M_d.size(); ++i)
        M_d[i] = 1./M_d[i];

    // 4) W = M_d^{-1} . I^{T} . M_s

    W.matrix().reserve( IM.matrix().nonZeros() ); // reserve same space as IM

    LinearAlgebra::backend().dsptd(M_d, IM.matrix(), M_s, W.matrix());
}

const char* Conservative::name() const {
    return  "conservative";
}

void Conservative::hash(eckit::MD5 &md5) const {
    FELinear::hash(md5);
}

void Conservative::print(std::ostream &out) const {
    out << "Conservative[]";
}

namespace {
static MethodBuilder< Conservative > __conservative("conservative");
}

}  // namespace method
}  // namespace mir

