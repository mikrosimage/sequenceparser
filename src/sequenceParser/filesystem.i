%include "common.i"

%include <std_vector.i>
%include <std_except.i>
%include <exception.i>

%{
#include "sequenceParser/Item.hpp"
#include "sequenceParser/filesystem.hpp"
#include <boost/exception/diagnostic_information.hpp>
%}

%template(ItemVector) ::std::vector<sequenceParser::Item>;

%include "filesystem.hpp"

namespace sequenceParser {
%ignore browse(
		const boost::filesystem::path&,
		const EDetection detectOptions,
		const std::vector<std::string>& );
}

