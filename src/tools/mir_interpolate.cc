#include <limits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"
#include "eckit/runtime/Tool.h"

#include "atlas/FieldSet.h"

#include "mir/mir_config.h"
#include "mir/FieldSink.h"
#include "mir/FieldSource.h"
#include "mir/Interpolate.h"

//------------------------------------------------------------------------------------------------------

using namespace eckit;
using namespace atlas;
using namespace mir;

//------------------------------------------------------------------------------------------------------

class MirInterpolate : public eckit::Tool {

	void run()
	{
		FieldSource source( ctxt_ );
		Interpolate interpolator( ctxt_ );
		FieldSink   sink( ctxt_ );

		FieldSet::Ptr fs_inp = source.eval(); ASSERT( fs_inp );

		FieldSet::Ptr fs_out = interpolator.eval( fs_inp ); ASSERT( fs_out );

		sink.eval( fs_out );
	}

public:

    MirInterpolate(int argc,char **argv): eckit::Tool(argc,argv)
    {
        MirContext* mir_ctxt = new MirContext();

        ValueParams* user( new ValueParams() );

        PathName path_in;
        path_in = Resource<std::string>("-i","");
        if( path_in.asString().empty() )
            throw UserError( "missing input filename, parameter -i", Here());

        user->set( "Input.Path", Value(path_in) );

        std::string oformat = Resource<std::string>("-oformat","grib");
        if( oformat.empty() )
            throw UserError( "bad output format, parameter -oformat", Here());

        user->set( "Target.OutputFormat", Value(oformat) );

        PathName path_out;
        path_out = Resource<std::string>("-o","");
        if( path_out.asString().empty() )
            throw UserError( "missing output filename, parameter -o", Here());

        user->set( "Target.Path", Value(path_out) );

        PathName clone_path;
        clone_path = Resource<std::string>("-g","");
        if( clone_path.asString().empty() )
            throw UserError( "missing clone grid filename, parameter -g", Here());

        user->set( "Target.GridPath", Value(clone_path) );

        std::string method = Resource<std::string>("-m;$MIR_METHOD","fe");

        user->set( "InterpolationMethod", method );

        mir_ctxt->push_front( Params::Ptr(user) );
        ctxt_.reset( mir_ctxt );
    }

private:

    eckit::Params::Ptr ctxt_;

};

//------------------------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
    MirInterpolate tool(argc,argv);
    tool.start();
    return 0;
}

