%include "commonDefinitions.i"

%include <global.i>
%include <std_vector.i>

%{
#include "filesystem.hpp"
%}

%template(ItemVector) ::std::vector<sequenceParser::Item>;

%include "filesystem.hpp"
