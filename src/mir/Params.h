/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date July 2014

#ifndef mir_Params_H
#define mir_Params_H

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/value/CompositeParams.h"
#include "eckit/value/DispatchParams.h"
#include "eckit/value/ValueParams.h"

//------------------------------------------------------------------------------------------------------

namespace mir {

//------------------------------------------------------------------------------------------------------

class UserParams : public eckit::DispatchParams<UserParams> {

public: // methods

    UserParams();

private: // methods

    eckit::Params::value_t getMaskPath( const eckit::Params::key_t& ) const;

};

//-------------------------------------------------------------------------------------------

class ProfileParams : public eckit::ValueParams {
public:

    ProfileParams()
    {
		set( "InterpolationMethod", "fe" );
//      set( "lsm", "hres.grib" );
    }
};

//-------------------------------------------------------------------------------------------

class MirParams : public eckit::CompositeParams {
public:

    MirParams();

};

//------------------------------------------------------------------------------------------------------

} // namespace mir

#endif
