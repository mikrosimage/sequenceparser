#ifndef _SEQUENCE_PARSER_FILESYSTEM_HPP_
#define _SEQUENCE_PARSER_FILESYSTEM_HPP_

#include "common.hpp"
#include "Item.hpp"
#include "Sequence.hpp"

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <iomanip>


namespace sequenceParser {

/**
 * @brief Browse your filesystem to detect the sequence from a pattern.
 * @param[out] outSequence: output sequence to create
 * @param[in] pattern: Absolute path of your sequence, like: "/tmp/foo####.jpg"
 * @param[in] accept: patterns to accept in the detection
 */
bool browseSequence( Sequence& outSequence, const std::string& pattern, const EPattern accept = ePatternDefault );


#ifndef SWIG
/**
 * @brief Browse the content of a directory on your filesystem, with the notion of Sequences.
 * @param[in] directory: the input directory in which it will search.
 * @param[in] detectOptions: some options to choose how to consider sequences.
 * @param[in] filters: set filters to limit the search.
 *                     For example to limit to jpg files, use "*.jpg".
 * @return A vector of files, sequences and directories.
 */
std::vector<Item> browse(
		const boost::filesystem::path& directory,
		const EDetection detectOptions = eDetectionDefault,
		const std::vector<std::string>& filters = std::vector<std::string>() );

#endif


inline std::vector<Item> browse(
		const std::string& directory,
		const EDetection detectOptions = eDetectionDefault,
		const std::vector<std::string>& filters = std::vector<std::string>() )
{
#ifdef SWIGJAVA
	return browse( boost::filesystem::path(utf8_to_latin1(directory)), detectOptions, filters );
#else
	return browse( boost::filesystem::path(directory), detectOptions, filters );
#endif
}


inline std::vector<Item> browse(
		const Item& directory,
		const EDetection detectOptions = eDetectionDefault,
		const std::vector<std::string>& filters = std::vector<std::string>() )
{
	return browse( directory.getPath(), detectOptions, filters );
}


}

#endif
