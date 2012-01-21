
%module sequenceParser

%include <std_vector.i>
%include <std_list.i>
%include <std_string.i>

%{
#include <Python.h>
#include "Detector.hpp"
#include "File.hpp"
#include "Folder.hpp"
#include "Sequence.hpp"
%}


namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
}

%include "Detector.hpp"


