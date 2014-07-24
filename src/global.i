
%include <std_vector.i>
%include <std_string.i>

namespace std {
%template(StringVector) vector<string>;
}
