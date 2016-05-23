%include "common.i"

%{
#include "sequenceParser/common.hpp"
#include "sequenceParser/Sequence.hpp"
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
                def __lt__(self, other):
                    return self.getFilenameWithStandardPattern() < other.getFilenameWithStandardPattern()
                def __eq__(self, other):
                    return self._prefix == other._prefix and self._suffix == other._suffix and self._maxPadding == other._maxPadding and self._fixedPadding == other._fixedPadding and self._ranges == other._ranges
        }
}

#endif
