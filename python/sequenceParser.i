
%module sequenceParser

%include <std_vector.i>
%include <std_list.i>
%include <std_string.i>

%{
#include <Python.h>
#include <include/Detector.hpp>
#include <src/File.hpp>
#include <src/Folder.hpp>
#include <src/Sequence.hpp>
%}


namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
}

%include <include/Detector.hpp>


