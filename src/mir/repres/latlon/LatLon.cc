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
/// @date Apr 2015


#include "mir/repres/latlon/LatLon.h"

#include <cmath>
#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace latlon {



LatLon::LatLon(const param::MIRParametrisation &parametrisation) :
    bbox_(parametrisation),
    increments_(parametrisation) {
    ASSERT(parametrisation.get("Ni", ni_));
    ASSERT(parametrisation.get("Nj", nj_));
}


LatLon::LatLon(const util::BoundingBox &bbox, const util::Increments &increments) :
    bbox_(bbox),
    increments_(increments) {
    setNiNj();
}


LatLon::~LatLon() {
}


void LatLon::cropToDomain(const param::MIRParametrisation &parametrisation, context::Context & ctx) const {
    if (!domain().isGlobal()) {
        action::AreaCropper cropper(parametrisation, bbox_);
        cropper.execute(ctx);
    }
}


void LatLon::setNiNj() {
    ni_ = bbox_.computeNi(increments_);
    nj_ = bbox_.computeNj(increments_);
}


bool LatLon::shiftedLon(const double& west, const double& east, const double& we) {

    // FIXME get precision from GRIB (angularPrecision)
    const double epsilon_grib1 = 1.0 / 1000.0;

    return    (eckit::types::is_approximately_equal(west, we/2., epsilon_grib1)
            && eckit::types::is_approximately_equal(east, 360 - we/2., epsilon_grib1));
}


bool LatLon::shiftedLat(const double& south, const double& north, const double& sn) {

    // FIXME get precision from GRIB (angularPrecision)
    const double epsilon_grib1 = 1.0 / 1000.0;

    return    (eckit::types::is_approximately_equal(north,  90. - sn/2., epsilon_grib1)
            && eckit::types::is_approximately_equal(south, -90. + sn/2., epsilon_grib1));
}


void LatLon::reorder(long scanningMode, std::vector<double> &values) const {
    // Code from ecRegrid, UNTESTED!!!

    eckit::Log::debug<LibMir>() << "WARNING: UNTESTED!!! ";
    eckit::Log::debug<LibMir>() << "LatLon::reorder scanning mode 0x" << std::hex << scanningMode << std::dec << std::endl;

    ASSERT(values.size() == ni_ * nj_);

    std::vector<double> out(values.size());

    if (scanningMode == jScansPositively) {
        size_t count = 0;
        for (int j = nj_ - 1 ; j >= 0; --j) {
            for (size_t i = 0 ; i <  ni_; ++i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == iScansNegatively) {
        size_t count = 0;
        for (size_t j = 0  ; j < nj_; ++j) {
            for (int i = ni_ - 1 ; i >= 0; --i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    if (scanningMode == (iScansNegatively | jScansPositively)) {
        size_t count = 0;
        for (int j = nj_ - 1  ; j >= 0; --j) {
            for (int i = ni_ - 1 ; i >= 0; --i) {
                out[count++] = values[j * ni_ + i];
            }
        }
        ASSERT(count == out.size());
        std::swap(values, out);
        return;
    }

    std::ostringstream os;
    os << "LatLon::reorder: unsupported scanning mode 0x" << std::hex << scanningMode;
    throw eckit::SeriousBug(os.str());
}


void LatLon::print(std::ostream &out) const {
    out << "bbox=" << bbox_
        << ",increments=" << increments_
        << ",ni=" << ni_
        << ",nj=" << nj_
        ;
}


void LatLon::fill(grib_info &info) const  {
    // See copy_spec_from_ksec.c in libemos for info
    // Warning: scanning mode not considered

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_LL;

    info.grid.Ni = ni_;
    info.grid.Nj = nj_;

    increments_.fill(info);
    bbox_.fill(info);
}


void LatLon::fill(api::MIRJob &job) const  {
    increments_.fill(job);
    bbox_.fill(job);
}


class LatLonIterator : public Iterator {

    size_t ni_;
    size_t nj_;

    eckit::Fraction north_;
    eckit::Fraction west_;
    eckit::Fraction lat_;
    eckit::Fraction lon_;

    eckit::Fraction we_;
    eckit::Fraction ns_;

    size_t i_;
    size_t j_;

    size_t count_;

    virtual void print(std::ostream &out) const {
        out << "LatLonIterator["
            <<  "ni="     << ni_
            << ",nj="     << nj_
            << ",north="  << north_
            << ",west="   << west_
            << ",we="     << we_
            << ",ns="     << ns_
            << ",i="      << i_
            << ",j="      << j_
            << ",count="  << count_
            << "]";
    }

    virtual bool next(double &lat, double &lon) {
        if (j_ < nj_) {
            if (i_ < ni_) {
                lat = lat_;
                lon = lon_;
                lon_ += we_;
                i_++;
                if (i_ == ni_) {
                    j_++;
                    lat_ -= ns_;
                    i_ = 0;
                    lon_ = west_;
                }
                count_++;
                return true;
            }
        }
        return false;
    }

public:
    LatLonIterator(size_t ni,
                   size_t nj,
                   double north,
                   double west,
                   double we,
                   double ns):
        ni_(ni),
        nj_(nj),
        north_(north),
        west_(west),
        lat_(north),
        lon_(west_),
        we_(we),
        ns_(ns),
        i_(0),
        j_(0),
        count_(0) {

    }

    ~LatLonIterator() {
        ASSERT(count_ == ni_ * nj_);
    }

};


Iterator *LatLon::unrotatedIterator() const {
    return new LatLonIterator(ni_,
                              nj_,
                              bbox_.north(),
                              bbox_.west(),
                              increments_.west_east(),
                              increments_.south_north());
}


Iterator* LatLon::rotatedIterator() const {
    return unrotatedIterator();
}


size_t LatLon::frame(std::vector<double> &values, size_t size, double missingValue) const {

    // Could be done better, just a demo
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < nj_; j++) {
        for (size_t i = 0; i < ni_; i++) {
            if ( !((i < size) || (j < size) || (i >= ni_ - size) || (j >= nj_ - size))) { // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;

}


void LatLon::validate(const std::vector<double> &values) const {
    eckit::Log::debug<LibMir>() << "LatLon::validate " << values.size() << " ni*nj " << ni_ * nj_ << std::endl;
    ASSERT(values.size() == ni_ * nj_);
}


void LatLon::shape(size_t &ni, size_t &nj) const {
    ni = ni_;
    nj = nj_;
}


util::Domain LatLon::domain() const {
    return domain(bbox_);
}


util::Domain LatLon::domain(const util::BoundingBox& bbox) const {
    using eckit::Fraction;

    Fraction sn(increments_.south_north());
    Fraction we(increments_.west_east());

    Fraction north = bbox.north();
    Fraction south = bbox.south();
    Fraction east = bbox.east();
    Fraction west = bbox.west();

    // correct if grid range is pole-to-pole, or is shifted South-North
    if ((north - south == 180) || shiftedLat(south, north, sn)) {
        north =  90;
        south = -90;
    }

    // correct if grid is periodic, or is shifted West-East
    if (east - west + we == 360 || shiftedLon(west, east, we)) {
        east = west + 360;
    }

    return util::Domain(north, west, south, east);
}


}  // namespace latlon
}  // namespace repres
}  // namespace mir

