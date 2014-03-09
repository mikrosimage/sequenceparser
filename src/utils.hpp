#ifndef _SEQUENCE_PARSER_UTILS_HPP_
#define _SEQUENCE_PARSER_UTILS_HPP_

#include "commonDefinitions.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>


namespace sequenceParser {

boost::regex convertFilterToRegex( std::string filter, const EDetection detectOptions );


std::vector<boost::regex> convertFilterToRegex( const std::vector<std::string>& filters, const EDetection detectOptions );


/**
 * Detect if the filename is filtered by one of the filter
 *
 * @param[in] filename filename need to be check if it filtered
 * @param[in] filters vector of filters
 * @param[in] detectOptions enable research options (Cf. EDetection in commonDefinitions.hpp )
 *
 * @return return true if the filename is filtered by filter(s)
 */
bool filenameRespectsFilters( const std::string& filename, const std::vector<boost::regex>& filters );


bool filepathRespectsAllFilters( const boost::filesystem::path& inputPath, const std::vector<boost::regex>& filters, const std::string& filename, const EDetection detectOptions );

}

#endif
