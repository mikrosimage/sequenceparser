%include "common.i"

%{
#include "sequenceParser/Item.hpp"
%}

%include "Item.hpp"

#ifdef SWIGPYTHON

%extend sequenceParser::Item
{
    %pythoncode
    {
        def __str__(self):
            return self.string()
        def __lt__(self, other):
            return self.getAbsoluteFilepath() < other.getAbsoluteFilepath()
    }
}

#endif
