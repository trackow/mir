/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Andreas Mueller
/// @author Pedro Maciel


#include <algorithm>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/mpi/Comm.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/FactoryOption.h"
#include "eckit/option/Option.h"
#include "eckit/option/Separator.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"
#include "eckit/testing/Test.h"

#include "atlas/grid.h"
#include "atlas/option.h"
#include "atlas/trans/Trans.h"

#include "mir/action/context/Context.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/data/MIRValuesVector.h"
#include "mir/input/GeoPointsFileInput.h"
#include "mir/input/GribFileInput.h"
#include "mir/input/MultiScalarInput.h"
#include "mir/namedgrids/NamedGrid.h"
#include "mir/output/MIROutput.h"
#include "mir/param/CombinedParametrisation.h"
#include "mir/param/ConfigurationWrapper.h"
#include "mir/param/DefaultParametrisation.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/repres/latlon/RegularLL.h"
#include "mir/repres/latlon/RotatedLL.h"
#include "mir/repres/other/UnstructuredGrid.h"
#include "mir/repres/sh/SphericalHarmonics.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/Pretty.h"
#include "mir/util/Rotation.h"
#include "mir/util/Wind.h"


class MIRSpectralTransform : public mir::tools::MIRTool {
private:
    void execute(const eckit::option::CmdArgs&);

    int minimumPositionalArguments() const { return 2; }

    void usage(const std::string& tool) const {
        eckit::Log::info() << "\n"
                              "Usage: "
                           << tool
                           << " --grid=WE/SN|--gridname=<namedgrid>|--griddef=<path> [--key1=value [--key2=value "
                              "[...]]] input.grib output.grib"
                              "\n"
                              "Examples: "
                              "\n"
                              "  % "
                           << tool
                           << " --grid=1/1 --area=90/-180/-90/179 in out"
                              "\n"
                              "  % "
                           << tool
                           << " --gridname=O32 --validate=false in out"
                              "\n"
                              "  % "
                           << tool << " --griddef=weather-params.pts in out" << std::endl;
    }

public:
    MIRSpectralTransform(int argc, char** argv) : mir::tools::MIRTool(argc, argv) {
        using eckit::option::Separator;
        using eckit::option::SimpleOption;
        using eckit::option::VectorOption;

        options_.push_back(new Separator("Output grid (mandatory one option)"));
        options_.push_back(
            new VectorOption<double>("grid", "Regular latitude/longitude grid increments (West-East/South-North)", 2));
        options_.push_back(new SimpleOption<std::string>("gridname", "Interpolate to given grid name"));
        options_.push_back(
            new SimpleOption<eckit::PathName>("griddef", "Path to file containing latitude/longitude pairs"));

        options_.push_back(new Separator("Output regular latitude/longitude grids"));
        options_.push_back(new VectorOption<double>(
            "area", "Regular latitude/longitude grid bounding box (North/West/South/East)", 4));
        options_.push_back(new VectorOption<double>(
            "rotation", "Regular latitude/longitude grid rotation by moving the South pole to latitude/longitude", 2));

        options_.push_back(new Separator("Miscellaneous"));
        options_.push_back(new SimpleOption<bool>(
            "vod2uv",
            "Input is vorticity and divergence (vo/d), convert to u/v components (gridded u/v or spectral U/V)"));
        options_.push_back(new SimpleOption<size_t>(
            "multi-scalar", "Number of fields (scalar or vo/d pairs) per Atlas/Trans instance (default 1)"));
        options_.push_back(new SimpleOption<size_t>(
            "multi-transform",
            "Number of fields  (scalar or vo/d pairs) per inverse transform (default is value of 'multi-scalar')"));
        options_.push_back(new SimpleOption<std::string>("atlas-trans-type",
                                                         "Atlas/Trans spectral transforms type (default 'local')"));
        options_.push_back(new SimpleOption<bool>("cesaro", "Cesàro summation filtering"));
        options_.push_back(new SimpleOption<double>("cesaro-k", "Cesàro summation k (default 2.)"));
        options_.push_back(new SimpleOption<size_t>(
            "cesaro-truncation", "Cesàro summation filtering minimum truncation (1 <= Tmin < T, default 1)"));
        options_.push_back(new SimpleOption<bool>("unstructured", "Atlas: force unstructured grid (default false)"));
        options_.push_back(new SimpleOption<bool>("caching", "MIR: caching (default true)"));
        options_.push_back(new SimpleOption<bool>("validate", "MIR: validate results (default false)"));
    }
};

const mir::repres::Representation* output_representation(std::ostream& log,
                                                         const mir::param::MIRParametrisation& parametrisation) {

    std::vector<double> grid;
    if (parametrisation.get("grid", grid)) {

        ASSERT(grid.size() == 2);
        mir::util::Increments increments(grid[0], grid[1]);

        mir::util::BoundingBox bbox;
        std::vector<double> area;
        if (parametrisation.get("area", area)) {
            ASSERT(area.size() == 4);
            bbox = mir::util::BoundingBox(area[0], area[1], area[2], area[3]);
        }

        log << "MIRSpectralTransform:"
            << "\n\t" << increments << "\n\t" << bbox
            << "\n\t"
               "shifted in latitude? "
            << increments.isLatitudeShifted(bbox)
            << "\n\t"
               "shifted in longitude? "
            << increments.isLongitudeShifted(bbox) << std::endl;

        std::vector<double> rot;
        if (parametrisation.get("rotation", rot)) {
            ASSERT(rot.size() == 2);
            mir::util::Rotation rotation(rot[0], rot[1]);

            return new mir::repres::latlon::RotatedLL(increments, rotation, bbox);
        }

        return new mir::repres::latlon::RegularLL(increments, bbox);
    }

    std::string griddef;
    if (parametrisation.get("griddef", griddef) && !griddef.empty()) {
        ASSERT(!parametrisation.has("rotation"));
        return new mir::repres::other::UnstructuredGrid(eckit::PathName(griddef));
    }

    std::string gridname;
    if (parametrisation.get("gridname", gridname) && !gridname.empty()) {
        ASSERT(!parametrisation.has("rotation"));
        return mir::namedgrids::NamedGrid::lookup(gridname).representation();
    }

    throw eckit::UserError("MIRSpectralTransform: could not create output representation");
}

atlas::Grid output_grid(const mir::param::MIRParametrisation& parametrisation,
                        const mir::repres::Representation& representation) {

    if (parametrisation.has("griddef") || parametrisation.has("unstructured")) {
        std::unique_ptr<mir::repres::Iterator> it(representation.iterator());

        std::vector<atlas::PointXY>* coordinates = new std::vector<atlas::PointXY>;
        coordinates->reserve(representation.count());

        while (it->next()) {
            const mir::Point2& p(*(*it));
            coordinates->push_back(atlas::PointXY(p[1], p[0]));
        }

        return atlas::UnstructuredGrid(coordinates);
    }

    return representation.atlasGrid();
}

void MIRSpectralTransform::execute(const eckit::option::CmdArgs& args) {
    eckit::ResourceUsage usage("MIRSpectralTransform");

    // Setup options
    static mir::param::DefaultParametrisation defaults;
    const mir::param::ConfigurationWrapper commandLine(args);

    size_t paramIdu = 0;
    size_t paramIdv = 0;
    mir::util::Wind::paramIds(commandLine, paramIdu, paramIdv);

    const bool vod2uv = args.getBool("vod2uv", false);
    const bool validate = args.getBool("validate", false);
    const bool cesaro = args.getBool("cesaro", false);

    const size_t multiScalar = args.getUnsigned("multi-scalar", 1);
    if (multiScalar < 1) {
        throw eckit::UserError("Option 'multi-scalar' has to be greater than or equal to one");
    }

    size_t multiTransform = args.getUnsigned("multi-transform", multiScalar);
    if (multiTransform < 1 || multiTransform > multiScalar) {
        throw eckit::UserError("Option 'multi-transform' has to be in range [1, " + std::to_string(multiScalar) + "] ('multi-scalar')");
    }

    if (args.has("grid") + args.has("gridname") + args.has("griddef") != 1) {
        throw eckit::UserError("Output description is required: either 'grid', 'gridname' or 'griddef'");
    }

    // Setup output (file)
    std::unique_ptr<mir::output::MIROutput> output(mir::output::MIROutputFactory::build(args(1), commandLine));
    ASSERT(output);

    // Setup input (file) and parametrisation
    std::unique_ptr<mir::input::MIRInput> input;
    {
        auto multi = new mir::input::MultiScalarInput();
        for (size_t i = 0; i < multiScalar; ++i) {
            if (vod2uv) {
                // vo/d field pairs
                multi->appendScalarInput(new mir::input::GribFileInput(args(0), i * 2, multiScalar * 2));
                multi->appendScalarInput(new mir::input::GribFileInput(args(0), i * 2 + 1, multiScalar * 2));
            } else {
                multi->appendScalarInput(new mir::input::GribFileInput(args(0), i, multiScalar));
            }
        }
        input.reset(multi);
    }
    ASSERT(input);

    const mir::param::CombinedParametrisation combined(commandLine, input->parametrisation(), defaults);
    auto& parametrisation = dynamic_cast<const mir::param::MIRParametrisation&>(combined);

    // Preserve statistics
    mir::util::MIRStatistics statistics;

    {
        auto& log = eckit::Log::info();

        eckit::Timer total_timer("Total time");

        const std::string what(vod2uv ? "vo/d field pair" : "field");

        size_t i = 0;
        while (input->next()) {
            log << "============> " << what << ": " << (++i * multiScalar) << std::endl;

            // Set representation and grid
            const mir::repres::Representation* outputRepresentation = output_representation(log, parametrisation);
            ASSERT(outputRepresentation);

            atlas::Grid outputGrid = output_grid(parametrisation, *outputRepresentation);
            ASSERT(outputGrid);

            mir::context::Context ctx(*input, statistics);
            mir::data::MIRField& field = ctx.field();

            size_t T = field.representation()->truncation();
            ASSERT(T > 0);

            size_t N = mir::repres::sh::SphericalHarmonics::number_of_complex_coefficients(T);
            ASSERT(N > 0);

            // Cesàro summation filtering
            if (cesaro) {
                eckit::Timer timer("time on Cesàro summation filtering", log);

                std::vector<double> filter(T + 1);
                {
                    double k = args.getDouble("cesaro-k", 2.);
                    size_t Tmin = args.getUnsigned("cesaro-truncation", 1);
                    ASSERT(1 <= Tmin && Tmin < T);

                    std::fill_n(filter.begin(), Tmin, 1.);
                    for (size_t n = Tmin; n <= T; ++n) {
                        auto a = double(T - n + 1);
                        auto f = filter[n - 1];
                        filter[n] = f * a / (a + k);
                    }
                }

                for (size_t d = 0; d < field.dimensions(); ++d) {
                    auto& values = field.direct(d);
                    ASSERT(values.size() == N * 2);

                    for (size_t m = 0, k = 0; m <= T; ++m) {
                        for (size_t n = m; n <= T; ++n) {
                            ASSERT(k + 1 < N * 2);
                            values[k++] *= filter[n];
                            values[k++] *= filter[n];
                        }
                    }
                }
            }

            // Set Trans
            std::string type = "local";
            parametrisation.get("atlas-trans-type", type);

            atlas::util::Config transConfig;
            transConfig.set("type", type);

            atlas::trans::Trans trans(outputGrid, int(T), transConfig);

            log << "MIRSpectralTransform:"
                << "\n\t"
                   "Atlas/Trans configuration type: "
                << transConfig.getString("type", "(default)")
                << "\n\t"
                   "Atlas unstructured grid: "
                << (atlas::UnstructuredGrid(outputGrid) ? "yes" : "no") << std::endl;

            if (vod2uv) {
                ASSERT(field.dimensions() == multiScalar * 2);

                size_t F;
                for (size_t numberOfFieldPairsProcessed = 0; numberOfFieldPairsProcessed < multiScalar;
                     numberOfFieldPairsProcessed += F) {
                    F = std::min(multiTransform, multiScalar - numberOfFieldPairsProcessed);
                    ASSERT(F > 0);

                    log << "MIRSpectralTransform: transforming " << mir::util::Pretty(int(F), what) << "..."
                        << std::endl;

                    // set input working area
                    // spectral coefficients are "interlaced", avoid copies if transforming only one field pair)
                    mir::MIRValuesVector input_vo;
                    mir::MIRValuesVector input_d;
                    if (F > 1) {
                        eckit::Timer timer("time on interlacing spectra", log);
                        input_vo.resize(F * N * 2);
                        input_d.resize(F * N * 2);

                        for (size_t f = 0; f < F; ++f) {
                            const size_t which = (numberOfFieldPairsProcessed + f) * 2;
                            mir::repres::sh::SphericalHarmonics::interlace_spectra(input_vo, field.values(which + 0), T,
                                                                                   N, f, F);
                            mir::repres::sh::SphericalHarmonics::interlace_spectra(input_d, field.values(which + 1), T,
                                                                                   N, f, F);
                        }
                    }

                    // set output working area
                    const size_t Ngp = size_t(outputGrid.size());
                    std::vector<double> out(F * Ngp * 2);

                    // inverse transform
                    {
                        eckit::Timer timer("time on invtrans", log);
                        const size_t which = numberOfFieldPairsProcessed * 2;
                        trans.invtrans(int(F), F > 1 ? input_vo.data() : field.values(which + 0).data(),
                                       F > 1 ? input_d.data() : field.values(which + 1).data(), out.data(),
                                       atlas::option::global());
                    }

                    // set field values, forcing u/v paramId (copies are necessary since fields are paired)
                    // Note: transformed u and v fields are contiguous, we save them in alternate order
                    {
                        eckit::Timer timer("time on copying grid-point values", log);

                        long u = long(paramIdu);
                        long v = long(paramIdv);

                        auto here = out.cbegin();
                        for (size_t f = 0; f < F; ++f) {
                            const size_t which = (numberOfFieldPairsProcessed + f) * 2;
                            mir::MIRValuesVector output_field(here, here + int(Ngp));

                            field.update(output_field, which);
                            field.metadata(which, "paramId", u);
                            here += int(Ngp);
                        }

                        for (size_t f = 0; f < F; ++f) {
                            const size_t which = (numberOfFieldPairsProcessed + f) * 2 + 1;
                            mir::MIRValuesVector output_field(here, here + int(Ngp));

                            field.update(output_field, which);
                            field.metadata(which, "paramId", v);
                            here += int(Ngp);
                        }
                    }
                }

            } else {
                ASSERT(field.dimensions() == multiScalar);

                size_t F;
                for (size_t numberOfFieldsProcessed = 0; numberOfFieldsProcessed < multiScalar;
                     numberOfFieldsProcessed += F) {
                    F = std::min(multiTransform, multiScalar - numberOfFieldsProcessed);
                    ASSERT(F > 0);

                    log << "MIRSpectralTransform: transforming " << mir::util::Pretty(int(F), what) << "..."
                        << std::endl;

                    // set input working area
                    // spectral coefficients are "interlaced", avoid copies if transforming only one field)
                    mir::MIRValuesVector in;
                    if (F > 1) {
                        eckit::Timer timer("time on interlacing spectra", log);
                        in.resize(F * N * 2);

                        for (size_t f = 0; f < F; ++f) {
                            mir::repres::sh::SphericalHarmonics::interlace_spectra(
                                in, field.values(numberOfFieldsProcessed + f), T, N, f, F);
                        }
                    }

                    // set output working area
                    const auto Ngp = outputGrid.size();
                    mir::MIRValuesVector out(F * size_t(Ngp));

                    // inverse transform
                    {
                        eckit::Timer timer("time on invtrans", log);
                        trans.invtrans(int(F), F > 1 ? in.data() : field.values(numberOfFieldsProcessed).data(),
                                       out.data(), atlas::option::global());
                    }

                    // set field values (again, avoid copies for one field only)
                    if (F > 1) {
                        eckit::Timer timer("time on copying grid-point values", log);

                        auto here = out.cbegin();
                        for (size_t f = 0; f < F; ++f) {
                            mir::MIRValuesVector output_field(here, here + Ngp);

                            field.update(output_field, numberOfFieldsProcessed + f);
                            here += Ngp;
                        }
                    } else {
                        field.update(out, numberOfFieldsProcessed);
                    }
                }
            }

            // set field representation
            field.representation(outputRepresentation);
            if (validate) {
                eckit::Timer timer("time on validate", log);
                field.validate();
            }

            // save
            output->save(parametrisation, ctx);
        }

        statistics.report(log);

        log << mir::util::Pretty(int(i * multiScalar), what) << " in " << eckit::Seconds(total_timer.elapsed())
            << ", rate: " << double(i) / double(total_timer.elapsed()) << " " << what << "/s" << std::endl;
    }
}

int main(int argc, char** argv) {
    MIRSpectralTransform tool(argc, argv);
    return tool.start();
}
