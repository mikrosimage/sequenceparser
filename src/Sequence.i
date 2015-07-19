%include "common.i"

%{
#include "common.hpp"
#include "Sequence.hpp"
%}

namespace std {
// Allow vector of object with no default constructor
%ignore vector< sequenceParser::FrameRange >::vector(size_type); 
%ignore vector< sequenceParser::FrameRange >::resize;

// Create instantiations of a template classes
%template(FrameRangeVector) vector< sequenceParser::FrameRange >;
}

%include "Sequence.hpp"

#ifdef SWIGPYTHON

%extend sequenceParser::Sequence
{
	%pythoncode
	{
		def __str__(self):
			return self.string()
	}
}

#endif
