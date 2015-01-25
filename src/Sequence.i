%include "common.i"

%{
#include "common.hpp"
#include "Sequence.hpp"
%}

%include "Sequence.hpp"

%extend sequenceParser::Sequence
{
	%pythoncode
	{
		def __str__(self):
			return self.string()
	}
}

