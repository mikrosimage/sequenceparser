#ifndef _SEQUENCE_PARSER_UTILS_HPP_
#define _SEQUENCE_PARSER_UTILS_HPP_

#include "commonDefinitions.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>


namespace sequenceParser {

boost::regex convertFilterToRegex( std::string filter, const EMaskOptions desc );


std::vector<boost::regex> convertFilterToRegex( const std::vector<std::string>& filters, const EMaskOptions desc );


/**
 * Detect if the filename is filtered by one of the filter
 *
 * @param[in] filename filename need to be check if it filtered
 * @param[in] filters vector of filters
 * @param[in] desc enable research options (Cf. EMaskOptions in commonDefinitions.hpp )
 *
 * @return return true if the filename is filtered by filter(s)
 */
bool filenameIsNotFilter( const std::string& filename, const std::vector<boost::regex>& filters );


bool isNotFilter( const boost::filesystem::path& inputPath, const std::vector<boost::regex>& filters, const std::string& filename, const EMaskOptions desc );

}

#endif
