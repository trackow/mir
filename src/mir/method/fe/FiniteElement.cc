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


#include "mir/method/fe/FiniteElement.h"

#include <algorithm>
#include <cmath>
#include <forward_list>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <utility>

#include "eckit/utils/MD5.h"
#include "eckit/utils/StringTools.h"

#include "mir/caching/InMemoryMeshCache.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/util/Domain.h"
#include "mir/util/Log.h"
#include "mir/util/Trace.h"


namespace mir {
namespace method {
namespace fe {


// epsilon used to scale edge tolerance when projecting ray to intesect element
static constexpr double parametricEpsilon = 1e-15;

static std::once_flag once;
static std::recursive_mutex* mtx                       = nullptr;
static std::map<std::string, FiniteElementFactory*>* m = nullptr;
static void init() {
    mtx = new std::recursive_mutex();
    m   = new std::map<std::string, FiniteElementFactory*>();
}

using triplet_vector_t    = std::vector<WeightMatrix::Triplet>;
using element_tree_t      = atlas::interpolation::method::ElemIndex3;
using failed_projection_t = std::pair<size_t, PointLatLon>;


struct element_t : std::vector<size_t> {
    using vector::vector;

    virtual bool intersects(const atlas::interpolation::method::Ray&) = 0;

    void append_triplets(size_t i, size_t nbRealPoints, triplet_vector_t& t) const {
        ASSERT(size() == weights.size());

        bool normalise = std::any_of(cbegin(), cend(), [nbRealPoints](size_t i) { return i >= nbRealPoints; });
        if (normalise) {
            // sum all calculated weights for normalisation
            double sum = 0.;
            size_t n   = 0;
            for (size_t j = 0; j < size(); ++j) {
                if (operator[](j) < nbRealPoints) {
                    sum += weights[j];
                    ++n;
                }
            }

            // normalise all weights according to the total (if no reasonable sum is found, distribute equitably)
            if (n > 0) {
                bool equitable = sum <= std::numeric_limits<double>::epsilon();
                auto invSum    = 1. / (equitable ? double(n) : sum);

                for (size_t j = 0; j < size() && 0 < n; ++j) {
                    if (operator[](j) < nbRealPoints) {
                        t.emplace_back(i, operator[](j), equitable ? invSum : (weights[j] * invSum));
                    }
                }
            }

            return;
        }

        for (size_t j = 0; j < size(); ++j) {
            t.emplace_back(i, operator[](j), weights[j]);
        }
    }

    std::vector<double>
        weights;  //< weights (per element vertex) are the linear Lagrange function at u,v (barycentric coordinates)
};


struct triag_t : element_t, atlas::interpolation::element::Triag3D {
    triag_t(const atlas::array::ArrayView<double, 2>& coords, size_t i1, size_t i2, size_t i3) :
        element_t{i1, i2, i3},
        Triag3D(atlas::PointXYZ{coords(i1, XYZCOORDS::XX), coords(i1, XYZCOORDS::YY), coords(i1, XYZCOORDS::ZZ)},
                atlas::PointXYZ{coords(i2, XYZCOORDS::XX), coords(i2, XYZCOORDS::YY), coords(i2, XYZCOORDS::ZZ)},
                atlas::PointXYZ{coords(i3, XYZCOORDS::XX), coords(i3, XYZCOORDS::YY), coords(i3, XYZCOORDS::ZZ)}) {}

    bool intersects(const atlas::interpolation::method::Ray& r) override {
        auto is = Triag3D::intersects(r, parametricEpsilon * std::sqrt(area()));
        if (is) {
            weights.assign({1. - is.u - is.v, is.u, is.v});
            return true;
        }
        return false;
    }
};


struct quad_t : element_t, atlas::interpolation::element::Quad3D {
    quad_t(const atlas::array::ArrayView<double, 2>& coords, size_t i1, size_t i2, size_t i3, size_t i4) :
        element_t{i1, i2, i3, i4},
        Quad3D(atlas::PointXYZ{coords(i1, XYZCOORDS::XX), coords(i1, XYZCOORDS::YY), coords(i1, XYZCOORDS::ZZ)},
               atlas::PointXYZ{coords(i2, XYZCOORDS::XX), coords(i2, XYZCOORDS::YY), coords(i2, XYZCOORDS::ZZ)},
               atlas::PointXYZ{coords(i3, XYZCOORDS::XX), coords(i3, XYZCOORDS::YY), coords(i3, XYZCOORDS::ZZ)},
               atlas::PointXYZ{coords(i4, XYZCOORDS::XX), coords(i4, XYZCOORDS::YY), coords(i4, XYZCOORDS::ZZ)}) {}

    bool intersects(const atlas::interpolation::method::Ray& r) override {
        auto is = Quad3D::intersects(r, parametricEpsilon * std::sqrt(area()));
        if (is) {
            weights.assign({(1. - is.u) * (1. - is.v), is.u * (1. - is.v), is.u * is.v, (1. - is.u) * is.v});
            return true;
        }
        return false;
    }
};


FiniteElement::FiniteElement(const param::MIRParametrisation& param, const std::string& label) :
    MethodWeighted(param), meshGeneratorParams_(param, label) {
    param.get("finite-element-validate-mesh", validateMesh_ = false);
    param.get("finite-element-missing-value-on-projection-fail", missingValueOnProjectionFail_ = true);

    // mesh requirements
    meshGeneratorParams_.meshCellCentres_ = true;
}


atlas::Mesh FiniteElement::atlasMesh(util::MIRStatistics& statistics, const repres::Representation& repres) const {

    // let representation set mesh generator parameters
    auto params = meshGeneratorParams_;
    repres.fill(params);

    double d;
    if (!repres.getLongestElementDiagonal(d)) {
        params.meshCellLongestDiagonal_ = true;
    }

    auto msh = caching::InMemoryMeshCache::atlasMesh(statistics, repres.atlasGrid(), params);
    if (!params.meshCellLongestDiagonal_) {
        ASSERT(d > 0.);
        msh.metadata().set("cell_longest_diagonal", d);
    }

    if (validateMesh_) {
        auto& connectivity = msh.cells().node_connectivity();
        auto& nodes        = msh.nodes();
        auto nb_nodes      = size_t(nodes.size());
        auto coords        = atlas::array::make_view<double, 2>(nodes.field("xyz"));

        size_t idx[4];
        for (atlas::idx_t i = 0; i < connectivity.rows(); ++i) {
            /*
             * Somewhat expensive sanity check, assumes:
             * - nb_cols == 3 implies triangle
             * - nb_cols == 4 implies quadrilateral (these are validated)
             * - no other element is supported at the time
             */
            const auto nb_cols = connectivity.cols(i);
            ASSERT(nb_cols == 3 || nb_cols == 4);

            for (atlas::idx_t j = 0; j < nb_cols; ++j) {
                idx[j] = size_t(connectivity(i, j));
                ASSERT(idx[j] < nb_nodes);
            }

            if (nb_cols == 4) {
                quad_t quad(coords, idx[0], idx[1], idx[2], idx[3]);
                if (!quad.validate()) {
                    Log::warning() << "Invalid Quad : " << quad << std::endl;
                    throw exception::SeriousBug("Found invalid quadrilateral in mesh", Here());
                }
            }
        }
    }

    return msh;
}


FiniteElement::~FiniteElement() = default;


void FiniteElement::print(std::ostream& out) const {
    out << "FiniteElement[name=" << name() << ",";
    MethodWeighted::print(out);
    out << ",validateMesh=" << validateMesh_ << ",missingValueOnProjectionFail=" << missingValueOnProjectionFail_
        << "]";
}


bool FiniteElement::sameAs(const Method& other) const {
    auto o = dynamic_cast<const FiniteElement*>(&other);
    return (o != nullptr) && meshGeneratorParams_.sameAs(o->meshGeneratorParams_) &&
           validateMesh_ == o->validateMesh_ && missingValueOnProjectionFail_ == o->missingValueOnProjectionFail_ &&
           MethodWeighted::sameAs(other);
}


void FiniteElement::hash(eckit::MD5& md5) const {
    MethodWeighted::hash(md5);
    meshGeneratorParams_.hash(md5);

    // FIXME uncomment on cache version increase
    // md5 << validateMesh_;
    // md5 << missingValueOnProjectionFail_;
}


void FiniteElement::assemble(util::MIRStatistics& statistics, WeightMatrix& W, const repres::Representation& in,
                             const repres::Representation& out) const {
    auto& log = Log::debug();

    log << "FiniteElement::assemble (input: " << in << ", output: " << out << ")" << std::endl;


    // get input mesh
    ASSERT(meshGeneratorParams().meshCellCentres_);  // required for the k-d tree
    const auto& inMesh = atlasMesh(statistics, in);


    // generate k-d tree with cell centres
    std::unique_ptr<element_tree_t> eTree;
    {
        trace::ResourceUsage timer("k-d tree: create");
        eTree.reset(atlas::interpolation::method::create_element_centre_index(inMesh));
    }

    double R = inMesh.metadata().getDouble("cell_longest_diagonal");
    ASSERT(R > 0.);
    log << "k-d tree: search radius R=" << R << "m" << std::endl;


    // grid/mesh properties
    const auto inDomain      = in.domain();
    const auto& connectivity = inMesh.cells().node_connectivity();
    const auto& inNodes      = inMesh.nodes();
    const auto inCoords      = atlas::array::make_view<double, 2>(inNodes.field("xyz"));

    const auto nbOutputPoints = out.numberOfPoints();
    const auto nbInputPoints  = size_t(inNodes.size());
    const auto nbRealPts =
        inNodes.metadata().has("NbRealPts") ? inNodes.metadata().get<size_t>("NbRealPts") : nbInputPoints;


    // some statistics
    size_t nbMaxElementsSearched   = 0;
    size_t nbMinElementsSearched   = std::numeric_limits<size_t>::max();
    size_t nbMaxProjectionAttempts = 0;
    size_t nbProjections           = 0;
    size_t nbFailures              = 0;

    std::forward_list<failed_projection_t> failures;
    triplet_vector_t weights_triplets;             // structure to fill-in sparse matrix
    weights_triplets.reserve(nbOutputPoints * 4);  // preallocate space as if all elements where quads


    {
        trace::ProgressTimer progress("Projecting", nbOutputPoints, {"point"}, log);

        // iterate over output points
        size_t ip = 0;
        for (std::unique_ptr<repres::Iterator> it(out.iterator()); it->next(); ++ip, ++progress) {
            ASSERT(ip < nbOutputPoints);

            if (inDomain.contains(it->pointRotated())) {

                // 3D projection, trying elements closest to p (if this fails, consider lowering parametricEpsilon)
                Point3 p(it->point3D());
                size_t nbProjectionAttempts = 0;

                auto closest = eTree->findInSphere(p, R);

                atlas::interpolation::method::Ray ray(p.data());

                bool success = false;
                for (const auto& close : closest) {
                    ++nbProjectionAttempts;

                    /*
                     * Assumes:
                     * - nb_cols == 3 implies triangle
                     * - nb_cols == 4 implies quadrilateral
                     * - no other element is supported at the time
                     */
                    const auto e = close.value().payload();
                    ASSERT(e < size_t(connectivity.rows()));

                    auto idx = [e, nbInputPoints, &connectivity](size_t j) {
                        auto x = size_t(connectivity(e, j));
                        ASSERT(x < nbInputPoints);
                        return x;
                    };

                    const auto nb_cols = connectivity.cols(e);

                    std::unique_ptr<element_t> elem(
                        nb_cols == 3   ? static_cast<element_t*>(new triag_t(inCoords, idx(0), idx(1), idx(2)))
                        : nb_cols == 4 ? new quad_t(inCoords, idx(0), idx(1), idx(2), idx(3))
                                       : NOTIMP);

                    if (elem->intersects(ray)) {
                        elem->append_triplets(ip, nbRealPts, weights_triplets);
                        success = true;
                        break;
                    }
                }

                nbMaxElementsSearched   = std::max(nbMaxElementsSearched, closest.size());
                nbMinElementsSearched   = std::min(nbMinElementsSearched, closest.size());
                nbMaxProjectionAttempts = std::max(nbMaxProjectionAttempts, nbProjectionAttempts);

                if (success) {
                    ++nbProjections;
                }
                else if (!missingValueOnProjectionFail_) {
                    failures.emplace_front(ip, it->pointUnrotated());
                    ++nbFailures;
                }
            }
        }
    }

    log << "Projected " << Log::Pretty(nbProjections) << " of " << Log::Pretty(nbOutputPoints, {"point"}) << "\n"
        << "k-d tree: searched between " << Log::Pretty(nbMinElementsSearched) << " and "
        << Log::Pretty(nbMaxElementsSearched, {"element"}) << ", with up to "
        << Log::Pretty(nbMaxProjectionAttempts, {"projection attempt"}) << " (per point)" << std::endl;

    if (nbFailures > 0) {
        std::stringstream msg;
        msg << "Failed to project " << Log::Pretty(nbFailures, {"point"});
        log << msg.str() << ":";
        size_t count = 0;
        for (const auto& f : failures) {
            log << "\n\tpoint " << f.first << " " << f.second;
            if (++count > 10) {
                log << "\n\t...";
                break;
            }
        }
        log << std::endl;
        throw exception::SeriousBug(msg.str());
    }


    // fill sparse matrix
    W.setFromTriplets(weights_triplets);
}


FiniteElementFactory::FiniteElementFactory(const std::string& name) : MethodFactory(name), name_(name) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*mtx);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("FiniteElementFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


FiniteElementFactory::~FiniteElementFactory() {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*mtx);

    m->erase(name_);
}


void FiniteElementFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*mtx);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


FiniteElement* FiniteElementFactory::build(std::string& names, const std::string& label,
                                           const param::MIRParametrisation& param) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*mtx);

    for (const auto& name : eckit::StringTools::split("/", names)) {
        Log::debug() << "FiniteElementFactory: looking for '" << name << "'" << std::endl;
        auto j = m->find(name);
        if (j != m->end()) {
            names = name;
            return j->second->make(param, label);
        }
    }

    list(Log::error() << "FiniteElementFactory: unknown '" << names << "', choices are: ");
    throw exception::SeriousBug("FiniteElementFactory: unknown '" + names + "'");
}


}  // namespace fe
}  // namespace method
}  // namespace mir
