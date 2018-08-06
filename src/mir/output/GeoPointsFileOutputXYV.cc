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
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/output/GeoPointsFileOutputXYV.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/HandleBuf.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/action/context/Context.h"
#include "mir/data/MIRField.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "eckit/serialisation/HandleStream.h"


namespace mir {
namespace output {


// See https://software.ecmwf.int/wiki/display/METV/Geopoints


GeoPointsFileOutputXYV::GeoPointsFileOutputXYV(const std::string& path, bool binary) :
    GeoPointsFileOutput(path, binary) {
}


static const char* keys[] = {"class", "type", "stream", "expver",
                             "date", "time", "step", "number", "levtype",
                             "levelist", "param"
                            };


size_t GeoPointsFileOutputXYV::copy(const param::MIRParametrisation&, context::Context&) {
    NOTIMP;
}


size_t GeoPointsFileOutputXYV::save(const param::MIRParametrisation& param,
                                    context::Context& ctx) {

    ASSERT(once());
    if (binary_) {
        return saveBinary(param, ctx);
    }
    else {
        return saveText(param, ctx);
    }
}

size_t GeoPointsFileOutputXYV::saveText(const param::MIRParametrisation& param,
                                        context::Context& ctx) {

    data::MIRField& field = ctx.field();

    eckit::DataHandle& handle = dataHandle();
    eckit::Offset position = handle.position();

    std::ostream out(new eckit::HandleBuf(handle));

    for (size_t j = 0; j < field.dimensions(); ++j) {

        // ASSERT(field.dimensions() == 1);

        param::RuntimeParametrisation runtime(param);
        auto md = field.metadata(j);
        if (md.find("paramId") != md.end()) {
            std::ostringstream oss;
            oss << md["paramId"];
            runtime.set("param", oss.str());
        }

        const MIRValuesVector& values = field.values(j);


        // eckit::Log::info() << "GeoPointsFileOutputXYV::save => " << handle << std::endl;


        out << "#GEO"
            "\n#FORMAT XYV";

        for (auto& key : keys) {
            std::string v;
            if (runtime.get(key, v)) {
                out << "\n# " << key << "=" << v;
            }
        }

        out << "\n#DATA";


        auto v = values.cbegin();

        eckit::ScopedPtr<repres::Iterator> it(field.representation()->iterator());
        while (it->next()) {
            const repres::Iterator::point_ll_t& p = it->pointUnrotated();
            ASSERT(v != values.cend());
            out << "\n" << p.lon.value() << ' ' << p.lat.value() << ' ' << *v;
            ++v;
        }
        ASSERT(v == values.cend());

        out << std::endl;
    }

    // eckit::Log::info() << "GeoPointsFileOutputXYV::save <= " << handle.position() - position << std::endl;

    return handle.position() - position;
}


size_t GeoPointsFileOutputXYV::saveBinary(const param::MIRParametrisation& param,
        context::Context& ctx) {

    data::MIRField& field = ctx.field();

    eckit::DataHandle& handle = dataHandle();
    eckit::Offset position = handle.position();

    eckit::HandleStream out(handle);

    for (size_t j = 0; j < field.dimensions(); ++j) {

        // ASSERT(field.dimensions() == 1);

        param::RuntimeParametrisation runtime(param);
        auto md = field.metadata(j);
        if (md.find("paramId") != md.end()) {
            std::ostringstream oss;
            oss << md["paramId"];
            runtime.set("param", oss.str());
        }

        const MIRValuesVector& values = field.values(j);

        // eckit::Log::info() << "GeoPointsFileOutputXYV::save => " << handle << std::endl;

        out << "GEO";
        out << "XYV";

        for (auto& key : keys) {
            std::string v;
            if (runtime.get(key, v)) {
                out << key << v;
            }
        }

        out << '-';

        auto v = values.cbegin();
        out << values.size();

        eckit::ScopedPtr<repres::Iterator> it(field.representation()->iterator());
        size_t i = 0;
        while (it->next()) {
            const repres::Iterator::point_ll_t& p = it->pointUnrotated();
            ASSERT(v != values.cend());
            out << double(p.lon.value())
                << double(p.lat.value())
                << double(*v);
            ++v;
            ++i;
        }
        ASSERT(v == values.cend());
        ASSERT(i == values.size());
    }

    out << "END";


    // eckit::Log::info() << "GeoPointsFileOutputXYV::save <= " << handle.position() - position << std::endl;

    return handle.position() - position;
}


static MIROutputBuilder<GeoPointsFileOutputXYV> output1("geopoints-xyv", {".gp", ".gpt", ".geo"});
static MIROutputBuilder<GeoPointsFileOutputXYV> output2("geopoints");


}  // namespace output
}  // namespace mir

