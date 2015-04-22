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


#include <iostream>

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "soyuz/param/MIRParametrisation.h"
#include "soyuz/data/MIRField.h"
#include "soyuz/util/Bitmap.h"

#include "soyuz/action/BitmapFilter.h"


namespace mir {
namespace action {


namespace {
static eckit::Mutex local_mutex;
static std::map< std::string, util::Bitmap *> cache;
}


BitmapFilter::BitmapFilter(const MIRParametrisation &parametrisation):
    Action(parametrisation) {

    std::string path;
    ASSERT(parametrisation.get("bitmap", path));

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    std::map<std::string, util::Bitmap *>::iterator j = cache.find(path);
    if (j == cache.end()) {
        bitmap_ = cache[path] = new util::Bitmap(path);
    } else {
        bitmap_ = (*j).second;
    }

}


BitmapFilter::~BitmapFilter() {
}


void BitmapFilter::print(std::ostream &out) const {
    out << "BitmapFilter[bitmap=" << *bitmap_ << "]";
}


void BitmapFilter::execute(data::MIRField &field) const {

    double missingValue = field.missingValue();
    std::vector<double> &values = field.values();

    if (values.size() != bitmap_->width() * bitmap_->height()) {
        eckit::StrStream os;
        os << "BitmapFilter::execute size mismatch: values=" << values.size()
           << ", bitmap=" << bitmap_->width() << "x" << bitmap_->height()
           << eckit::StrStream::ends;

        throw eckit::UserError(std::string(os));
    }


    size_t k = 0;

    for (size_t j = 0; j < bitmap_->height() ; j++ ) {
        for (size_t i = 0; i < bitmap_->width() ; i++ ) {
            if (!bitmap_->on(i, j)) {
                values[k] = missingValue;
            }
            k++;
        }
    }

    field.hasMissing(true);

}


namespace {
static ActionBuilder< BitmapFilter > bitmapFilter("filter.bitmap");
}


}  // namespace action
}  // namespace mir

