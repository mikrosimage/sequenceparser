%include "common.i"

%{
#include "sequenceParser/Item.hpp"
%}

#ifdef SWIGJAVA

%rename (toString) sequenceParser::Item::string;
%ignore sequenceParser::Item::operator<;

#elif SWIGPYTHON

%rename(__str__) sequenceParser::Item::string;
%rename(__lt__) sequenceParser::Item::operator<;

#endif

%include "Item.hpp"
