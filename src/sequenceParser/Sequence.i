%include "common.i"

%{
#include "sequenceParser/common.hpp"
#include "sequenceParser/Sequence.hpp"
%}

namespace std {
// Allow vector of object with no default constructor
%ignore vector< sequenceParser::FrameRange >::vector(size_type);
%ignore vector< sequenceParser::FrameRange >::resize;

// Ignore the default constructor
%ignore pair::pair();

// Create instantiations of a template classes
%template(FrameRangeVector) vector< sequenceParser::FrameRange >;
%template(TimePair) pair< sequenceParser::Time, sequenceParser::Time >;
}

#ifdef SWIGJAVA

%rename (toString) sequenceParser::Sequence::string;
%rename (equals) sequenceParser::Sequence::operator==;
%ignore sequenceParser::Sequence::operator!=;
%ignore sequenceParser::Sequence::operator<;

#elif SWIGPYTHON

%rename(__str__) sequenceParser::Sequence::string;
%rename(__eq__) sequenceParser::Sequence::operator==;
%rename(__ne__) sequenceParser::Sequence::operator!=;
%rename(__lt__) sequenceParser::Sequence::operator<;

#endif

%include "Sequence.hpp"
