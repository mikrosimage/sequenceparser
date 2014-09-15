#ifndef _SEQUENCE_PARSER_UTILS_HPP_
#define _SEQUENCE_PARSER_UTILS_HPP_

#include "common.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>


namespace sequenceParser {

/**
 * @brief Convert a user filter into a regex.
 * A user filter looks like: "foo###.jpg", "foo@.tiff" or "foo%04d.jpg".
 */
boost::regex convertFilterToRegex( const std::string& filter, const EDetection detectOptions );

/**
 * @brief Convert user filters into regexes.
 * @see convertFilterToRegex
 */
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
