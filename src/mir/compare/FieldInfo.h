/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   FieldInfo.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef mir_compare_FieldInfo_h
#define mir_compare_FieldInfo_h

#include <iosfwd>
#include <string>
#include <map>
#include <vector>


namespace mir {
namespace compare {
class FieldSet;
}
}


namespace mir {
namespace compare {


class FieldInfo {
public:

    FieldInfo(const std::string& path, off_t offset, size_t length);

    off_t offset() const;

    size_t length() const;

    const std::string& path() const;

private:

    std::string path_;
    off_t offset_;
    size_t length_;

    void print(std::ostream &out) const;

    friend std::ostream &operator<<(std::ostream &s, const FieldInfo &x) {
        x.print(s);
        return s;
    }

};


}  // namespace compare
}  // namespace mir


#endif

