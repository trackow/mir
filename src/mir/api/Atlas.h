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


#ifndef mir_api_Atlas_H
#define mir_api_Atlas_H

#include <vector>
#include <array>

#include "mir/api/mir_config.h"

#ifdef HAVE_ATLAS

#include "atlas/grid/Grid.h"
#include "atlas/option/Options.h"
#include "atlas/projection/Projection.h"
#include "atlas/trans/LegendreCacheCreator.h"
#include "atlas/trans/Trans.h"
#include "atlas/util/Earth.h"
#include "atlas/util/GaussianLatitudes.h"
#include "atlas/util/Rotation.h"

#else

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"


namespace atlas {


typedef eckit::geometry::Point2 PointXY;
typedef eckit::geometry::Point3 PointXYZ;

struct PointLonLat : eckit::geometry::Point2 {
    PointLonLat();
    PointLonLat(double, double);
    double lat() const;
    double lon() const;
};


namespace util {
void gaussian_latitudes_npole_spole(int, double*);
struct Config {
    template<class T>
    void set(const char*, T) {}
};
struct Earth {
    static double radius();
    static double distance(const PointLonLat&, const PointLonLat&);
    static double distance(const PointXYZ&, const PointXYZ&);
    static void convertSphericalToCartesian(const double& radius, const Point2& Alonlat, Point3& B, double height = 0.);
};
struct Rotation {
    Rotation(const PointLonLat&);
    bool rotated() const;
    void rotate(double[]) const;
    void unrotate(double[]) const;
    friend std::ostream& operator<< (std::ostream&, const Rotation&) {}
};
}



class RectangularDomain {
public:
    RectangularDomain(const std::array<double, 2>&, const std::array<double, 2>&) {}
};

struct Projection {
    Projection() {}

    template<class T>
    Projection(T) {}

    operator bool() const;

    template<class T>
    PointLonLat lonlat(T) const {
        return PointLonLat();
    }
};



class Grid {
public:

Grid(){}
  template<class T>
        Grid(T) {}

    operator bool() const;
    const std::vector<long>& nx() const;
    Projection projection() const;
    util::Config spec() const;
};


namespace grid {

struct ReducedGaussianGrid : public Grid {
    template<class T>
    ReducedGaussianGrid(T) {}

    template<class T, class U>
    ReducedGaussianGrid(T, U) {}
};


struct RegularGaussianGrid : public Grid {
    template<class T>
    RegularGaussianGrid(T) {}

    template<class T, class U>
    RegularGaussianGrid(T, U) {}
};

struct LinearSpacing {
    LinearSpacing(const std::array<double, 2>&, size_t = 0) {}
    template<class T, class U, class V, class W>
    LinearSpacing(T, U, V, W) {}

    template<class T, class U, class V>
    LinearSpacing(T, U, V) {}

};

struct StructuredGrid : public Grid {

    typedef Projection Projection;

    struct XSpace {
        template<class T>
        XSpace(T) {}

        template<class T, class U>
        XSpace(T, U) {}

        XSpace(const std::array<double, 2>&, const std::vector<long>&, bool) {}
    };
    struct YSpace {
        template<class T>
        YSpace(T) {}

        template<class T, class U>
        YSpace(T, U) {}
    };

    template<class T>
    StructuredGrid(T) {}

    template<class T, class U>
    StructuredGrid(T, U) {}
     template<class T, class U, class V, class W>
    StructuredGrid(T, U, V, W) {}
};

struct UnstructuredGrid : public Grid {
template<class T>
    UnstructuredGrid(T) {}
};

}

}


#endif


#endif

