%include <std_vector.i>
%include <std_string.i>
%include <std_except.i>
%include <exception.i>

namespace std {
%template(StringVector) vector<string>;
}

%{
#include "sequenceParser/common.hpp"
#include <boost/filesystem/operations.hpp>
%}

namespace sequenceParser {
typedef int Time;
}

%exception {
try
{
	$action
}
catch (boost::filesystem::filesystem_error& e)
{
	std::string message;
	message = e.code().message() + " : ";
 	if (!e.path1().empty())
        message += e.path1().string() + ".";
		
 	if (!e.path2().empty())
        message += " " + e.path2().string();
	SWIG_exception( SWIG_IOError, message.c_str() );	
}
catch (std::exception& e)
{
	SWIG_exception( SWIG_RuntimeError, e.what() );	
}
catch( ... )
{
	// Add the C++ backtrace into the exception log.
	SWIG_exception( SWIG_RuntimeError, "[sequence parser] unknown error" );
}

}

%include "common.hpp"
