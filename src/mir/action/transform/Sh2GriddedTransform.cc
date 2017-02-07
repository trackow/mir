/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @author Tiago Quintino
///
/// @date Apr 2015


#include "mir/action/transform/Sh2GriddedTransform.h"

#include <iostream>
#include <vector>
#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileLock.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/MD5.h"
#include "atlas/atlas.h"
#include "atlas/grid/Grid.h"
#include "atlas/grid/Structured.h"
#include "atlas/grid/grids.h"
#include "atlas/grid/lonlat/RegularLonLat.h"
#include "mir/action/context/Context.h"
#include "mir/action/transform/TransCache.h"
#include "mir/action/transform/TransInitor.h"
#include "mir/caching/InMemoryCache.h"
#include "mir/caching/LegendreCache.h"
#include "mir/caching/legendre/LegendreLoader.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"

#ifdef ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif


namespace mir {
namespace action {
namespace transform {

#ifdef ATLAS_HAVE_TRANS
namespace {


static eckit::Mutex amutex;
static mir::InMemoryCache<TransCache> trans_handles("mirCoefficient",
        8L * 1024 * 1024 * 1024,
        "$MIR_COEFFICIENT_CACHE",
        false); // Don't cleanup at exit: the Fortran part will dump core

static void fillTrans(struct Trans_t &trans,
                      size_t truncation,
                      const atlas::grid::Grid &grid) {

    const atlas::grid::Structured* reduced = dynamic_cast<const atlas::grid::Structured*>(&grid);

    if (!reduced) {
        throw eckit::SeriousBug("Spherical harmonics transforms only supports SH to ReducedGG/RegularGG/RegularLL.");
    }

    const atlas::grid::lonlat::RegularLonLat* latlon = dynamic_cast<const atlas::grid::lonlat::RegularLonLat* >(&grid);


    ASSERT(trans_new(&trans) == 0);

    ASSERT(trans_set_trunc(&trans, truncation) == 0);

    if (latlon) {
        ASSERT(trans_set_resol_lonlat(&trans, latlon->nlon(), latlon->nlat()) == 0);
    } else {

        const std::vector<long>& pl = reduced->pl();
        ASSERT(pl.size());

        std::vector<int> pli(pl.size());
        ASSERT(pl.size() == pli.size());

        for (size_t i = 0; i < pl.size(); ++i) {
            pli[i] = pl[i];
        }

        ASSERT(trans_set_resol(&trans, pli.size(), &pli[0]) == 0);
    }
}

static void createCoefficients(const eckit::PathName& path,
                               size_t truncation,
                               const atlas::grid::Grid &grid,
                               context::Context& ctx) {
    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().createCoeffTiming_);

    struct Trans_t tmp_trans;
    fillTrans(tmp_trans, truncation, grid);

    ASSERT(trans_set_write(&tmp_trans, path.asString().c_str()) == 0);
    ASSERT(trans_setup(&tmp_trans) == 0); // This will create the cache

    trans_delete(&tmp_trans);
}

static void transform(
    const std::string& key,
    const param::MIRParametrisation &parametrisation,
    size_t truncation,
    const std::vector<double> &input, std::vector<double> &output,
    const atlas::grid::Grid &grid,
    context::Context& ctx) {


    if (trans_handles.find(key) == trans_handles.end()) {

        eckit::PathName path;

        {   // Block for timers

            eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().coefficientTiming_);

            class LegendreCacheCreator: public caching::LegendreCache::CacheContentCreator {

                size_t truncation_;
                const atlas::grid::Grid & grid_;
                context::Context & ctx_;

                virtual void create(const eckit::PathName& path, int& ignore) {
                    createCoefficients(path, truncation_, grid_, ctx_);
                }
            public:
                LegendreCacheCreator(size_t truncation,
                                     const atlas::grid::Grid & grid,
                                     context::Context & ctx):
                    truncation_(truncation), grid_(grid), ctx_(ctx) {}
            };

            static caching::LegendreCache cache;
            LegendreCacheCreator creator(truncation, grid, ctx);

            int dummy = 0;
            path = cache.getOrCreate(key, creator, dummy);
        }

        {   // Block for timers

            eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().loadCoeffTiming_);

            eckit::Timer timer("Loading coefficients");

            TransCache &tc = trans_handles[key];

            struct Trans_t &trans = tc.trans_;
            fillTrans(trans, truncation, grid);

            tc.inited_ = true;
            tc.loader_ = caching::legendre::LegendreLoaderFactory::build(parametrisation, path);
            // std::cout << "LegendreLoader " << *tc.loader_ << std::endl;

            ASSERT(trans_set_cache(&trans, tc.loader_->address(), tc.loader_->size()) == 0);

            ASSERT(trans_setup(&trans) == 0);
        }

        // trans_handles.footprint(key, after - before);


    }

    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().sh2gridTiming_);
    eckit::Timer timer("SH2GRID");


    // Transform sp to gp fields =====================================

    TransCache &tc = trans_handles[key];
    struct Trans_t &trans = tc.trans_;

    ASSERT(trans.myproc == 1);
    ASSERT(trans.nspec2g == (int) input.size());

    int number_of_fields = 1;
    output.resize(size_t(number_of_fields * trans.ngptotg));

    struct InvTrans_t invtrans = new_invtrans(&trans);
    invtrans.nscalar   = number_of_fields;
    invtrans.rspscalar = input.data();
    invtrans.rgp       = output.data();
    ASSERT(trans_invtrans(&invtrans) == 0);


    // trans_delete(&trans);
}


}  // (anonymous namespace)
#endif


static void transform(const param::MIRParametrisation &parametrisation, size_t truncation,
                      const std::vector<double> &input, std::vector<double> &output,
                      const atlas::grid::Grid &grid,
                      context::Context& ctx) {
    eckit::AutoLock<eckit::Mutex> lock(amutex); // To protect trans_handles

    TransInitor::instance(); // Will init trans if needed

    std::ostringstream os;
    os << "T" << truncation << ":" << grid.uniqueId();
    std::string key(os.str());

    try {
        transform(key, parametrisation, truncation, input, output, grid, ctx);
    } catch (std::exception& e) {
        eckit::Log::error() << "Error while running SH2GRID: " << e.what() << std::endl;
        trans_handles.erase(key);
        throw;
    }
}


Sh2GriddedTransform::Sh2GriddedTransform(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {
}


Sh2GriddedTransform::~Sh2GriddedTransform() {
}


void Sh2GriddedTransform::execute(context::Context & ctx) const {
    // ASSERT(field.dimensions() == 1); // For now

    // Make sure another thread to no evict anything from the cache while we are using it
    InMemoryCacheUser<TransCache> use(trans_handles, ctx.statistics().transHandleCache_);

    data::MIRField& field = ctx.field();
    repres::RepresentationHandle out(outputRepresentation());

    // TODO: Transform all the fields together
    for (size_t i = 0; i < field.dimensions(); i++) {

        const std::vector<double> &values = field.values(i);
        std::vector<double> result;

        eckit::ScopedPtr<atlas::grid::Grid> grid(out->atlasGrid());
        transform(parametrisation_,
                  field.representation()->truncation(),
                  values,
                  result,
                  *grid,
                  ctx);

        field.update(result, i);

    }

    field.representation(out);
}


}  // namespace transform
}  // namespace action
}  // namespace mir

