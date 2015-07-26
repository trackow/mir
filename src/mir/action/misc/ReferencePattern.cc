/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "mir/action/misc/ReferencePattern.h"

#include <iostream>
#include <cmath>

#include "eckit/memory/ScopedPtr.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"

namespace mir {
namespace action {


ReferencePattern::ReferencePattern(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {
}


ReferencePattern::~ReferencePattern() {
}


void ReferencePattern::print(std::ostream &out) const {
    out << "ReferencePattern[]";
}


void ReferencePattern::execute(data::MIRField &field) const {

    repres::RepresentationHandle representation(field.representation());
    bool normalize = false;
    parametrisation_.get("0-1", normalize);

    bool hasMissing = field.hasMissing();
    double missingValue = field.missingValue();

    for (size_t k = 0; k < field.dimensions(); k++) {
        std::vector<double> &values = field.values(k);

        double minvalue = 0;
        double maxvalue = 0;

        size_t first = 0;
        size_t count = 0;
        for (; first < values.size(); ++first) {
            if (!hasMissing || values[first] != missingValue) {
                minvalue = values[first];
                maxvalue = values[first];
                count++;
                break;
            }
        }

        if (first == values.size()) {
            // Only missing values
            continue;
        }

        for (size_t i = first; i < values.size(); ++i) {
            if (!hasMissing || values[i] != missingValue) {
                minvalue = std::min(minvalue, values[i]);
                maxvalue = std::max(maxvalue, values[i]);
                count++;
            }
        }

        if (normalize) {
            maxvalue = 1;
            minvalue = 0;
        }

        double median = (minvalue + maxvalue) / 2;
        double range = maxvalue - minvalue;

        eckit::ScopedPtr<repres::Iterator> iter(representation->iterator(false));
        double lat = 0;
        double lon = 0;


        size_t j = 0;
        const double deg2rad = M_PI / 180.0;

        while (iter->next(lat, lon)) {

            if (!hasMissing || values[j] != missingValue) {
                values[j] = range * sin(3 * lon * deg2rad) * cos(3 * lat * deg2rad) * 0.5 + median;
            }

            j++;
        }

        ASSERT(j == values.size());

    }
}


namespace {
static ActionBuilder< ReferencePattern > action("misc.pattern");
}


}  // namespace action
}  // namespace mir

