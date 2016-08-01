%include <std_pair.i>
%include <std_vector.i>
%include <std_string.i>
%include <std_except.i>
%include <exception.i>
%include <typemaps.i>

namespace std {
%template(StringVector) vector<string>;
}

%{
#include "sequenceParser/common.hpp"
#include <boost/filesystem/operations.hpp>
%}

namespace sequenceParser {
typedef long int Time;
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

#ifdef SWIGJAVA
// Define some typemaps to be applied to std::string& arguments
%typemap(jstype) std::string& OUTPUT "String[]"
%typemap(jtype) std::string& OUTPUT "String[]"
%typemap(jni) std::string& OUTPUT "jobjectArray"
%typemap(javain)  std::string& OUTPUT "$javainput"

%typemap(in) std::string& OUTPUT (std::string temp) {
  if (!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
    return $null;
  }
  if (JCALL1(GetArrayLength, jenv, $input) == 0) {
    SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
  }
  $1 = &temp;
}

%typemap(argout) std::string& OUTPUT {
  jstring jvalue = JCALL1(NewStringUTF, jenv, temp$argnum.c_str()); 
  JCALL3(SetObjectArrayElement, jenv, $input, 0, jvalue);
}
#endif

%include "common.hpp"
