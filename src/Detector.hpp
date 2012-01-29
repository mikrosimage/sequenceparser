#ifndef _SEQUENCE_PARSER_DETECTOR_HPP_
#define _SEQUENCE_PARSER_DETECTOR_HPP_

#include "commonDefinitions.hpp"
#include "FileObject.hpp"
#include "Sequence.hpp"
#include "File.hpp"
#include "Folder.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <iomanip>

namespace sequenceParser {

namespace detail {
class FileStrings;
class FileNumbers;
}

class Detector
{

public:
	Detector();
	~Detector();

	/**
	 ** fileInDirectory methods
	 **
	 **/
	std::list<boost::shared_ptr<File> > fileInDirectory( const std::string& directory,
														 const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<File> > fileInDirectory( const std::string& directory,
														 std::vector<std::string>& filters,
														 const EMaskOptions desc = eMaskOptionsDefault );

	void printFileInDirectory( const std::string& directory,
							   const EMaskOptions desc = eMaskOptionsDefault );

	void printFileInDirectory( const std::string& directory,
							   std::vector<std::string>& filters,
							   const EMaskOptions desc = eMaskOptionsDefault );


	std::list<boost::shared_ptr<Sequence> > sequenceInDirectory( const std::string& directory,
																 const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<Sequence> > sequenceInDirectory( const std::string& directory,
																 std::vector<std::string>& filters,
																 const EMaskOptions desc = eMaskOptionsDefault );


	std::list<boost::shared_ptr<Sequence> > sequenceFromFilenameList( const std::vector<boost::filesystem::path>& filenames, const EMaskOptions desc = eMaskOptionsNone );


	void printSequenceInDirectory( const std::string& directory,
								   const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<FileObject> > fileAndSequenceInDirectory( const std::string& directory,
																		  const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<FileObject> > fileAndSequenceInDirectory( const std::string& directory,
																		  std::vector<std::string>& filters,
																		  const EMaskOptions desc = eMaskOptionsDefault );

	void printFileAndSequenceInDirectory( const std::string& directory,
										  const EMaskOptions desc = eMaskOptionsDefault );

	void printFileAndSequenceInDirectory( const std::string& directory,
										  std::vector<std::string>& filters,
										  const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<Folder> > folderInDirectory( const std::string& directory,
															 const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<Folder> > folderInDirectory( const std::string& directory,
															 std::vector<std::string>& filters,
															 const EMaskOptions desc = eMaskOptionsDefault );

	void printFolderInDirectory( const std::string& directory,
								 const EMaskOptions desc = eMaskOptionsDefault );

	void printFolderInDirectory( const std::string& directory,
								 std::vector<std::string>& filters,
								 const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<FileObject> > fileObjectInDirectory( const std::string& directory,
																	 const EMaskType mask = eMaskTypeDefault,
																	 const EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<FileObject> > fileObjectInDirectory( const std::string& directory,
																	 std::vector<std::string>& filters,
																	 const EMaskType mask = eMaskTypeDefault,
																	 const EMaskOptions desc = eMaskOptionsDefault );

	void printFileObjectInDirectory( const std::string& directory,
									 const EMaskType mask = eMaskTypeDefault,
									 const EMaskOptions desc = eMaskOptionsDefault );

	void printFileObjectInDirectory( const std::string& directory,
									 std::vector<std::string>& filters,
									 const EMaskType mask = eMaskTypeDefault,
									 const EMaskOptions desc = eMaskOptionsDefault );

private:
	/**
	 * Detect if the researchPath is a correct pattern ( a folder, a file or a pattern with an valid parent directory )
	 *
	 * @param[inout] reseachPath path/pattern to be checked, return the valid research directory
	 * @param[inout] filters to add pattern of detection present in the researchPath
	 *
	 * @return return false if the research path is incorrect with a folder, a file or a pattern with an valid parent directory
	 */
	bool detectDirectoryInResearch( std::string& researchPath, std::vector<std::string>& filters );

	
	/**
	 * @brief Create a list of sequences from a list of FileNumbers associated to a common FileStrings.
	 * 
	 * @param[in] stringParts the sequence identifier
	 * @param[inout] numberParts the list of sequence of number inside each filename
	 * @param[in] desc detection flags
	 * 
	 * @warning We modify @p numberParts in place, because we don't use it after
	 *          so there is no reason to create a copy.
	 * @return a sequence object with all informations
	 */
	std::list<Sequence> buildSequence( const boost::filesystem::path& directory, const detail::FileStrings& stringParts, std::list<detail::FileNumbers>& numberParts, const EMaskOptions& desc );

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
	 * @param[out] stringParts list of strings
	 * @param[out] numberParts list of integers
	 * 
	 * @return number of decteted numbers
	 */
	std::size_t decomposeFilename( const std::string& filename, detail::FileStrings& stringParts, detail::FileNumbers& numberParts, const EMaskOptions& options );
};

}

#endif
