%include "common.i"

%include <std_vector.i>

%{
#include "sequenceParser/common.hpp"
#include "sequenceParser/Sequence.hpp"
#include "sequenceParser/detector.hpp"
%}

/*
%template(FileObjectVector) ::boost::ptr_vector<sequenceParser::FileObject>;
%template(FileVector) ::boost::ptr_vector<sequenceParser::File>;
%template(FolderVector) ::boost::ptr_vector<sequenceParser::Folder>;
%template(SequenceVector) ::boost::ptr_vector<sequenceParser::Sequence>;
*/

%include "detector.hpp"


