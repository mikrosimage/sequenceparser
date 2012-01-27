
%include <std_vector.i>
%include <std_list.i>
%include <std_string.i>

namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
}
