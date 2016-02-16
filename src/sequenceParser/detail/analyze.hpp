#ifndef _SEQUENCE_PARSER_DETAIL_ANALYZE_HPP_
#define _SEQUENCE_PARSER_DETAIL_ANALYZE_HPP_

#include <sequenceParser/common.hpp>
#include <sequenceParser/Sequence.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <iomanip>

namespace sequenceParser {

namespace detail {
class FileStrings;
class FileNumbers;
}

/**
 * Detect if the researchPath is a correct pattern ( a folder, a file or a pattern with a valid parent directory )
 *
 * @param[inout] reseachPath path/pattern to be checked, return the valid research directory
 * @param[inout] filters to add pattern of detection present in the researchPath
 *
 * @return return false if the research path is incorrect with a folder, a file or a pattern with a valid parent directory
 */
bool detectDirectoryInResearch( std::string& researchPath, std::vector<std::string>& filters, std::string &filename );


Sequence privateBuildSequence(
		const Sequence& defaultSeq,
		const detail::FileStrings& stringParts,
		const std::vector<detail::FileNumbers>::const_iterator& numberPartsBegin,
		const std::vector<detail::FileNumbers>::const_iterator& numberPartsEnd,
		const std::size_t index,
		const std::size_t padding,
		const std::size_t maxPadding
	);


void privateBuildSequencesAccordingToPadding(
	std::vector<Sequence>& result,
	const Sequence& defaultSeq,
	const detail::FileStrings& stringParts,
	const std::vector<detail::FileNumbers>::iterator& numberPartsBegin,
	const std::vector<detail::FileNumbers>::iterator numberPartsEnd,
	const int index );


/**
 * @brief Create a vector of sequences from a vector of FileNumbers associated to a common FileStrings.
 * 
 * @param[in] stringParts the sequence identifier
 * @param[inout] numberParts the vector of sequence of number inside each filename
 * @param[in] detectOptions detection flags
 * 
 * @warning We modify @p numberParts in place, because we don't use it after
 *          so there is no reason to create a copy.
 * @return a sequence object with all informations
 */
std::vector<Sequence> buildSequences( const boost::filesystem::path& directory, const detail::FileStrings& stringParts, std::vector<detail::FileNumbers>& numberParts, const EDetection detectOptions );

/**
 * @brief Extract number and string parts from a filename.
 * 
 * Example:
 * filename = "aa1b22cccc3"
 * Will return:
 * stringParts = ["aa", "b", "cccc"]
 * numberParts = [1, 22, 3]
 * 
 * @param[in] str the string to process (filename)
 * @param[out] stringParts vector of strings
 * @param[out] numberParts vector of integers
 * 
 * @return number of decteted numbers
 */
std::size_t decomposeFilename( const std::string& filename, detail::FileStrings& stringParts, detail::FileNumbers& numberParts, const EDetection& options );

}

#endif
