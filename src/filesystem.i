%include "commonDefinitions.i"

%include <global.i>
%include <std_vector.i>
%include <std_except.i>
%include <exception.i>

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

%{
#include "filesystem.hpp"
#include <boost/exception/diagnostic_information.hpp>
%}

%template(ItemVector) ::std::vector<sequenceParser::Item>;

%include "filesystem.hpp"

namespace sequenceParser {
%ignore browse(
		const boost::filesystem::path&,
		const std::vector<std::string>&,
		const EDetection detectOptions,
		const EDisplay displayOptions );

%ignore browse(
		const boost::filesystem::path&,
		const EDetection detectOptions,
		const EDisplay displayOptions );
}

