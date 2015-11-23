/*
 * (C) Copyright 1996-2015 ECMWF.
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

#ifndef mir_util_Compare_H
#define mir_util_Compare_H

#include <cstddef>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

namespace mir {
namespace util {
namespace compare {

/// Compare values parent type (abstract comparison functor)
template< typename T >
struct compare_fn {
    virtual bool operator()(const T& v) const = 0;
};


/// Avoid comparing
template< typename T >
struct is_anything_fn : compare_fn<T> {
    bool operator()(const T& v) const {
        return true;
    }
};


/// Compare values equality
template< typename T >
struct is_equal_fn : compare_fn<T> {
    is_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
        return (v == ref);
    }
    const T ref;
};


/// Compare values equality, approximately
template< typename T >
struct is_approx_equal_fn : compare_fn<T> {
    is_approx_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
        return eckit::FloatCompare<T>::isApproximatelyEqual(v,ref);
    }
    const T ref;
};


/// Compare values inequality, "is greater or equal to"
template< typename T >
struct is_greater_equal_fn : compare_fn<T> {
    is_greater_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
        return (v >= ref);
    }
    const T ref;
};


/// Compare values inequality, "is greater or approximately equal to"
template< typename T >
struct is_approx_greater_equal_fn : compare_fn<T> {
    is_approx_greater_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
#if 0
        // FIXME: What should it be? give me a resource
        return ( a >= b )? || (fabs(a-b) < 1e-10);
#endif
        return (v >= ref) || eckit::FloatCompare<T>::isApproximatelyEqual(v,ref);
    }
    const T ref;
};


/// Compare values inequality, "is less than or equal to"
template< typename T >
struct is_less_equal_fn : compare_fn<T> {
    is_less_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
        return (v <= ref);
    }
    const T ref;
};


/// Compare values inequality, "is less than or approximately equal to"
template< typename T >
struct is_approx_less_equal_fn : compare_fn<T> {
    is_approx_less_equal_fn(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const {
        return (v <= ref) || eckit::FloatCompare<T>::isApproximatelyEqual(v,ref);
    }
    const T ref;
};


/// Compare if in mask (vector indices)
struct is_masked_fn : compare_fn< size_t > {
    is_masked_fn(const std::vector< bool >& mask_) : mask(mask_) {}
    bool operator()(const size_t& i) const {
        // TODO: remove when properly debugged
        ASSERT(i<mask.size());
        return mask[i];
    }
    const std::vector< bool >& mask;
};


/// Compare if not in mask (vector indices)
struct is_not_masked_fn : compare_fn< size_t > {
    is_not_masked_fn(const std::vector< bool >& mask_) : mask(mask_) {}
    bool operator()(const size_t& i) const {
        // TODO: remove when properly debugged
        ASSERT(i<mask.size());
        return !mask[i];
    }
    const std::vector< bool >& mask;
};


/// Utility comparators
extern const is_approx_equal_fn< double > is_approx_zero;
extern const is_approx_equal_fn< double > is_approx_one;


}  // namespace compare
}  // namespace util
}  // namespace mir

#endif
