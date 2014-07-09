/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/value/Properties.h"
#include "eckit/io/Buffer.h"

#include "atlas/grid/FieldSet.h"

#include "mir/mars_api.h"

#include "mir/Interpolate.h"

using namespace mir;
using namespace atlas::grid;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

class MarsContext : public eckit::Properties {

public: // methods

    MarsContext() : frozen_(false) {}

    bool frozen() const { return frozen_; }
    void freeze() { frozen_ = true; }

private: // members

    bool frozen_;

};

//------------------------------------------------------------------------------------------------------
//

extern "C" {

mir_err mir_create_context( mir_context_ptr* ctxt )
{
    MarsContext* mars_ctxt = new MarsContext();
    ASSERT(mars_ctxt);
    *ctxt = reinterpret_cast<mir_context_ptr>(mars_ctxt);
    return MIR_SUCCESS;
}

mir_err mir_set_context_logger(mir_context_ptr ctxt, logger_proc logger )
{
    ///< @todo regist the logger for the library
    NOTIMP;
    return MIR_SUCCESS;
}

mir_err mir_set_context_value(mir_context_ptr ctxt, const char* key, const char* value)
{
    if(!ctxt) return MIR_INVALID_CONTEXT;

    MarsContext* mctxt = reinterpret_cast<MarsContext*>(ctxt);

    if( mctxt->frozen() )
        return MIR_CONTEXT_FROZEN;

    mctxt->set(key,value);

    return MIR_SUCCESS;
}

mir_err mir_interpolate(mir_context_ptr ctxt, const void* buffin, size_t sin, void ** buffout, size_t* sout)
{
    if(!ctxt) return MIR_INVALID_CONTEXT;

    MarsContext* mctxt = reinterpret_cast<MarsContext*>(ctxt);

    if(!mctxt->frozen()) mctxt->freeze();

    bool shared = false;

    try
    {
        Buffer b(const_cast<void*>(buffin), sin, shared);

        FieldSet::Ptr fs_inp( new FieldSet(b) ); ///< @todo create a fieldset from a buffer

        Interpolate interpolator( *mctxt );

        FieldSet::Ptr fs_out = interpolator.eval( fs_inp );

        FieldHandle::Data& f = fs_out->fields().at(0)->data();  // NOTE: we assume only one field as output
        (*buffout) = f.data();
        (*sout)    = f.extent(0);                               // NOTE: we assume that the array is with 1 rank (single column vector)

        ASSERT( fs_out );
    }
    catch( eckit::Exception& e )
    {
        std::cout << e.what() << std::endl;
        return MIR_FAILED;
    }

    return MIR_SUCCESS;
}

mir_err mir_destroy_context(mir_context_ptr ctxt)
{
    if(!ctxt) return MIR_INVALID_CONTEXT;

    MarsContext* mctxt = reinterpret_cast<MarsContext*>(ctxt);
    delete mctxt;
    return MIR_SUCCESS;
}

};

//------------------------------------------------------------------------------------------------------
