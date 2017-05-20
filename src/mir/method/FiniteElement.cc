/*
 * (C) Copyright 1996-2016 ECMWF.
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


#include "mir/method/FiniteElement.h"

#include <algorithm>
#include <limits>
#include <list>
#include "eckit/config/Resource.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/ETA.h"
#include "eckit/log/Plural.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/MD5.h"
#include "atlas/grid/Structured.h"
#include "atlas/interpolation/element/Quad2D.h"
#include "atlas/interpolation/element/Quad3D.h"
#include "atlas/interpolation/element/Triag3D.h"
#include "atlas/interpolation/element/Triag2D.h"
#include "atlas/interpolation/method/PointIndex3.h"
#include "atlas/interpolation/method/Ray.h"
#include "atlas/mesh/ElementType.h"
#include "atlas/mesh/Elements.h"
#include "atlas/mesh/Nodes.h"
#include "atlas/mesh/actions/BuildCellCentres.h"
#include "atlas/mesh/actions/BuildXYZField.h"
#include "atlas/util/io/Gmsh.h"
#include "mir/config/LibMir.h"
#include "mir/method/GridSpace.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {


namespace {


// try to project to 20% of total number elements before giving up
static const double maxFractionElemsToTry = 0.2;

// epsilon used to scale edge tolerance when projecting ray to intesect element
static const double parametricEpsilon = 1e-16;


enum { LON=0, LAT=1 };


typedef std::vector< WeightMatrix::Triplet > Triplets;


struct MeshStats {

    size_t inp_ncells;
    size_t inp_npts;
    size_t out_npts;

    MeshStats(): inp_ncells(0), inp_npts(0), out_npts(0) {}

    void print(std::ostream &s) const {
        s << "MeshStats["
              "nb_cells=" << eckit::BigNum(inp_ncells)
          << ",inp_npts=" << eckit::BigNum(inp_npts)
          << ",out_npts=" << eckit::BigNum(out_npts)
          << "]";
    }

    friend std::ostream &operator<<(std::ostream &s, const MeshStats &p) {
        p.print(s);
        return s;
    }
};


static void normalise(Triplets& triplets)
{
    // sum all calculated weights for normalisation
    double sum = 0.0;

    for (size_t j = 0; j < triplets.size(); ++j) {
        sum += triplets[j].value();
    }

    // now normalise all weights according to the total
    const double invSum = 1.0 / sum;
    for (size_t j = 0; j < triplets.size(); ++j) {
        triplets[j].value() *= invSum;
    }
}


/// Find in which element the point is contained by projecting the point with each nearest element
static Triplets projectPointTo3DElements(
        const MeshStats &stats,
        const atlas::array::ArrayView<double, 2> &icoords,
        const atlas::mesh::Connectivity& connectivity,
        const FiniteElement::Point &p,
        size_t ip,
        size_t firstVirtualPoint,
        atlas::interpolation::method::ElemIndex3::NodeList::const_iterator start,
        atlas::interpolation::method::ElemIndex3::NodeList::const_iterator finish ) {

    ASSERT(start != finish);

    Triplets triplets;

    bool mustNormalise = false;
    size_t idx[4];
    double w[4];
    atlas::interpolation::method::Ray ray( p.data() );

    for (atlas::interpolation::method::ElemIndex3::NodeList::const_iterator itc = start; itc != finish; ++itc) {

        const size_t elem_id = (*itc).value().payload();
        ASSERT(elem_id < connectivity.rows());

        /* assumes:
         * - nb_cols == 3 implies triangle
         * - nb_cols == 4 implies quadrilateral
         * - no other element is supported at the time
         */
        const size_t nb_cols = connectivity.cols(elem_id);
        ASSERT(nb_cols == 3 || nb_cols == 4);

        for (size_t i = 0; i < nb_cols; ++i) {
            idx[i] = size_t(connectivity(elem_id, i));
            ASSERT(idx[i] < stats.inp_npts);
        }

        if (nb_cols == 3) {

            /* triangle */
            atlas::interpolation::element::Triag3D triag(
                    icoords[idx[0]].data(),
                    icoords[idx[1]].data(),
                    icoords[idx[2]].data());

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon * std::sqrt(triag.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::method::Intersect is = triag.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the linear Lagrange function evaluated at u,v (aka barycentric coordinates)
                w[0] = 1. - is.u - is.v;
                w[1] = is.u;
                w[2] = is.v;

                for (size_t i = 0; i < 3; ++i)
                {
                    if(idx[i] < firstVirtualPoint)
                        triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                    else
                        mustNormalise = true;
                }

                break; // stop looking for elements
            }

        } else {

            /* quadrilateral */
            atlas::interpolation::element::Quad3D quad(
                    icoords[idx[0]].data(),
                    icoords[idx[1]].data(),
                    icoords[idx[2]].data(),
                    icoords[idx[3]].data() );

            if ( !quad.validate() ) { // somewhat expensive sanity check
                eckit::Log::warning() << "Invalid Quad : " << quad << std::endl;
                throw eckit::SeriousBug("Found invalid quadrilateral in mesh", Here());
            }

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon * std::sqrt(quad.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::method::Intersect is = quad.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the bilinear Lagrange function evaluated at u,v
                w[0] = (1. - is.u) * (1. - is.v);
                w[1] =       is.u  * (1. - is.v);
                w[2] =       is.u  *       is.v ;
                w[3] = (1. - is.u) *       is.v ;


                for (size_t i = 0; i < 4; ++i) {
                    if(idx[i] < firstVirtualPoint)
                        triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                    else
                        mustNormalise = true;
                }

                break; // stop looking for elements
            }
        }

    } // loop over nearest elements

    // at least one of the nodes of element shouldn't be virtual
    if (!triplets.empty() && mustNormalise) {
        normalise(triplets);
    }

    return triplets;
}


/// Find in which element the point is contained by projecting the point with each nearest element
static Triplets projectPointTo2DElements(
        const MeshStats &stats,
        const atlas::array::ArrayView<double, 2> &ilonlat,
        const atlas::mesh::Connectivity& connectivity,
        const eckit::geometry::Point2 &p,
        size_t ip,
        atlas::interpolation::method::ElemIndex3::NodeList::const_iterator start,
        atlas::interpolation::method::ElemIndex3::NodeList::const_iterator finish ) {

    ASSERT(start != finish);

    Triplets triplets;

    size_t idx[4];
    double w[4];
    atlas::interpolation::Vector2D ray = atlas::interpolation::Vector2D::Map( p.data() );

    for (atlas::interpolation::method::ElemIndex3::NodeList::const_iterator itc = start; itc != finish; ++itc) {

        const size_t elem_id = (*itc).value().payload();
        ASSERT(elem_id < connectivity.rows());

        /* assumes:
         * - nb_cols == 3 implies triangle
         * - nb_cols == 4 implies quadrilateral
         * - no other element is supported at the time
         */
        const size_t nb_cols = connectivity.cols(elem_id);
        ASSERT(nb_cols == 3 || nb_cols == 4);

        for (size_t i = 0; i < nb_cols; ++i) {
            idx[i] = size_t(connectivity(elem_id, i));
            ASSERT(idx[i] < stats.inp_npts);
        }

        if (nb_cols == 3) {

            /* triangle */
            atlas::interpolation::element::Triag2D triag(
                    ilonlat[idx[0]].data(),
                    ilonlat[idx[1]].data(),
                    ilonlat[idx[2]].data());

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon;  // FIXME * std::sqrt(triag.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::method::Intersect is = triag.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the linear Lagrange function evaluated at u,v (aka barycentric coordinates)
                w[0] = 1. - is.u - is.v;
                w[1] = is.u;
                w[2] = is.v;

                for (size_t i = 0; i < 3; ++i) {
                        triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                }

                break; // stop looking for elements
            }

        } else {

            /* quadrilateral */
            atlas::interpolation::element::Quad2D quad(
                    ilonlat[idx[0]].data(),
                    ilonlat[idx[1]].data(),
                    ilonlat[idx[2]].data(),
                    ilonlat[idx[3]].data() );

            if ( !quad.validate() ) { // somewhat expensive sanity check
                eckit::Log::warning() << "Invalid Quad : " << quad << std::endl;
                throw eckit::SeriousBug("Found invalid quadrilateral in mesh", Here());
            }

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon * std::sqrt(quad.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::method::Intersect is = quad.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the bilinear Lagrange function evaluated at u,v
                w[0] = (1. - is.u) * (1. - is.v);
                w[1] =       is.u  * (1. - is.v);
                w[2] =       is.u  *       is.v ;
                w[3] = (1. - is.u) *       is.v ;


                for (size_t i = 0; i < 4; ++i) {
                    triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                }

                break; // stop looking for elements
            }
        }

    } // loop over nearest elements

    return triplets;
}


}  // (anonymous namespace)


FiniteElement::MeshGenParams::MeshGenParams() {
    set("three_dimensional", true);
    set("patch_pole",        true);
    set("include_pole",      false);
    set("angle",             0.);
    set("triangulate",       false);
}


FiniteElement::FiniteElement(const param::MIRParametrisation &param) :
    MethodWeighted(param) {
}


FiniteElement::~FiniteElement() {
}


void FiniteElement::hash(eckit::MD5&) const {
}


void FiniteElement::assemble(context::Context& ctx, WeightMatrix &W, const GridSpace& in, const GridSpace& out) const {

    // FIXME: arguments
    eckit::Log::debug<LibMir>() << "FiniteElement::assemble" << std::endl;

    eckit::Log::debug<LibMir>() << "  Input  Grid: " << in.grid()  << std::endl;
    eckit::Log::debug<LibMir>() << "  Output Grid: " << out.grid() << std::endl;

    const atlas::grid::Domain &inDomain = in.grid().domain();

    eckit::TraceTimer<LibMir> timer("Compute weights");

    // FIXME: using the name() is not the right thing, although it should work, but create too many cached meshes.
    // We need to use the mesh-generator
    {
        eckit::TraceTimer<LibMir> timer("Generate mesh");

        // generateMeshAndCache(in.grid(), in.mesh()); // mesh generation will be done lazily

        static bool dumpMesh = eckit::Resource<bool>("$MIR_DUMP_MESH", false);
        if (dumpMesh) {
            atlas::util::io::Gmsh gmsh;
            gmsh.options.set<std::string>("nodes", "xyz");

            eckit::Log::debug<LibMir>() << "Dumping input mesh to input.msh" << std::endl;
            gmsh.write(in.mesh(), "input.msh");

            eckit::Log::debug<LibMir>() << "Dumping output mesh to output.msh" << std::endl;
            atlas::mesh::actions::BuildXYZField("xyz")(out.mesh());
            gmsh.write(out.mesh(), "output.msh");
        }
    }

    // generate barycenters of each triangle & insert them on a kd-tree
    {
        eckit::ResourceUsage usage("create_cell_centres");
        eckit::TraceTimer<LibMir> timer("Tesselation::create_cell_centres");
        atlas::mesh::actions::BuildCellCentres()(in.mesh());
    }

    eckit::ScopedPtr<atlas::interpolation::method::ElemIndex3> eTree;
    {
        eckit::ResourceUsage usage("create_element_centre_index");
        eckit::TraceTimer<LibMir> timer("create_element_centre_index");
        eTree.reset( atlas::interpolation::method::create_element_centre_index(in.mesh()) );
    }

    // input mesh
    MeshStats stats;

    const atlas::mesh::Nodes  &i_nodes  = in.mesh().nodes();
    atlas::array::ArrayView<double, 2> icoords  ( i_nodes.field( "xyz" ));
    atlas::array::ArrayView<double, 2> ilonlat = in.coordsLonLat();

    size_t firstVirtualPoint = std::numeric_limits<size_t>::max();
    if (i_nodes.metadata().has("NbRealPts")) {
        firstVirtualPoint = i_nodes.metadata().get<size_t>("NbRealPts");
    }

    atlas::array::ArrayView<double, 2> ocoords = out.coordsXYZ();
    atlas::array::ArrayView<double, 2> olonlat = out.coordsLonLat();

    stats.inp_ncells = in.mesh().cells().size();
    stats.inp_npts   = i_nodes.size();
    stats.out_npts   = out.grid().npts();

    eckit::Log::debug<LibMir>() << stats << std::endl;

    // weights -- one per vertex of element, triangles (3) or quads (4)

    std::vector< WeightMatrix::Triplet > weights_triplets; // structure to fill-in sparse matrix
    weights_triplets.reserve( stats.out_npts * 4 );        // preallocate space as if all elements where quads

    // search nearest k cell centres

    const size_t maxNbElemsToTry = std::max<size_t>(64, stats.inp_ncells * maxFractionElemsToTry);
    size_t max_neighbours = 0;

    eckit::Log::debug<LibMir>() << "Projecting " << eckit::Plural(stats.out_npts, "output point") << " to input mesh " << in.grid().shortName() << std::endl;
    size_t Nsuccesses = 0;
    std::list<size_t> failures;
    {
        eckit::TraceTimer<LibMir> timerProj("Projecting");

        for ( size_t ip = 0; ip < stats.out_npts; ++ip ) {

            if (ip && (ip % 10000 == 0)) {
                double rate = ip / timerProj.elapsed();
                eckit::Log::debug<LibMir>()
                        << eckit::BigNum(ip) << " ..."  << eckit::Seconds(timerProj.elapsed())
                        << ", rate: " << rate << " points/s, ETA: "
                        << eckit::ETA( (stats.out_npts - ip) / rate )
                        << std::endl;
            }

            if (inDomain.contains(olonlat[ip][LON], olonlat[ip][LAT])) {

                // lookup point
                Point p ( ocoords[ip].data() );

                size_t kpts = 1;
                bool success = false;
                while (!success && kpts <= maxNbElemsToTry) {
                    max_neighbours = std::max(kpts, max_neighbours);

                    atlas::interpolation::method::ElemIndex3::NodeList cs = eTree->kNearestNeighbours(p, kpts);
                    Triplets triplets = projectPointTo3DElements(
                                stats,
                                icoords,
                                in.mesh().cells().node_connectivity(),
                                p,
                                ip,
                                firstVirtualPoint,
                                cs.begin(),
                                cs.end() );

                    if (triplets.size()) {
                        std::copy(triplets.begin(), triplets.end(), std::back_inserter(weights_triplets));
                        success = true;
                        ++Nsuccesses;
                    }
                    kpts *= 2;

                }

                if (!success) {
                    // If this fails, consider lowering atlas::grid::parametricEpsilon
                    failures.push_front(ip);
                }
            }
        }
    }
    size_t Nfailures = failures.size();
    eckit::Log::debug<LibMir>() << "Projected " << eckit::BigNum(Nsuccesses) << " of " << eckit::Plural(stats.out_npts, "point")
                                << ", with " << eckit::Plural(Nfailures, "projection failure") << std::endl;


    if (Nfailures) {
        const size_t goodIndex = std::numeric_limits<size_t>::max();
        eckit::TraceTimer<LibMir> timerProj("Recovering");

        for (size_t& ip : failures) {
            if (inDomain.contains(olonlat[ip][LON], olonlat[ip][LAT])) {

                // lookup point
                Point p (ocoords[ip].data());

                size_t kpts = 1;
                while (ip != goodIndex && kpts <= maxNbElemsToTry) {
                    max_neighbours = std::max(kpts, max_neighbours);

                    atlas::interpolation::method::ElemIndex3::NodeList cs = eTree->kNearestNeighbours(p, kpts);
                    Triplets triplets = projectPointTo2DElements(
                                stats,
                                ilonlat,
                                in.mesh().cells().node_connectivity(),
                                eckit::geometry::Point2(olonlat[ip].data()),
                                ip,
                                cs.begin(),
                                cs.end() );

                    if (triplets.size()) {
                        // triplets have to be in row-order (follows Triplet::operator<)
                        Triplets::const_iterator here = std::upper_bound(weights_triplets.begin(), weights_triplets.end(), triplets[0]);
                        weights_triplets.insert(here, triplets.begin(), triplets.end());
                        ip = goodIndex;
                    }
                    kpts *= 2;

                }
            }
        }

        // clear up the failed projections with the recoveries
        failures.remove(goodIndex);
        eckit::Log::debug<LibMir>() << "Recovered " << eckit::Plural(Nfailures - failures.size(), "projection failure") << std::endl;
        Nfailures = failures.size();
    }


    eckit::Log::debug<LibMir>() << "Maximum neighbours searched was " << eckit::Plural(max_neighbours, "element") << std::endl;
    if (!failures.empty()) {
        std::stringstream msg;
        msg << "Failed to project " << eckit::Plural(Nfailures, "point") << " out of " << eckit::Plural(stats.out_npts, "point");

        eckit::Log::debug<LibMir>() << msg.str() << ":";
        for (const size_t& ip: failures) {
            eckit::Log::debug<LibMir>() << "\n\tpoint " << ip << " (lon, lat) = (" << olonlat[ip][LON] << ", " << olonlat[ip][LAT];
        }
        eckit::Log::debug<LibMir>() << std::endl;
        throw eckit::SeriousBug(msg.str());
    }


    // fill sparse matrix
    W.setFromTriplets(weights_triplets);
}


void FiniteElement::generateMesh(const atlas::grid::Grid &grid, atlas::mesh::Mesh &mesh) const {

    eckit::ResourceUsage usage("FiniteElement::generateMesh");


    std::string meshgenerator( grid.getOptimalMeshGenerator() );

    parametrisation_.get("meshgenerator", meshgenerator); // Override with MIRParametrisation

    eckit::Log::debug<LibMir>() << "MeshGenerator parametrisation is '" << meshgenerator << "'" << std::endl;

    eckit::ScopedPtr<atlas::mesh::generators::MeshGenerator> generator(
                atlas::mesh::generators::MeshGeneratorFactory::build(meshgenerator, meshgenparams_) );
    generator->generate(grid, mesh);

    // If meshgenerator did not create xyz field already, do it now.
    atlas::mesh::actions::BuildXYZField()(mesh);
}


}  // namespace method
}  // namespace mir

