#ifndef _SEQUENCE_PARSER_DETECTOR_HPP_
#define _SEQUENCE_PARSER_DETECTOR_HPP_

#include <FileObject.hpp>
#include <FileStrings.hpp>
#include <FileNumbers.hpp>
#include <Sequence.hpp>
#include <File.hpp>
#include <Folder.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>

namespace sequenceParser {

class Detector
{
public :
	Detector();
	~Detector();

	/**
	 ** fileInDirectory methods
	 **
	 **/
#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory              ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory              ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  fileInDirectory              ( std::string path );
#endif
#ifndef SWIG
	void                                                      printFileInDirectory         ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	void                                                      printFileInDirectory         ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	void                                                      printFileInDirectory         ( std::string path );
#endif

#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory          ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory          ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  sequenceInDirectory          ( std::string path );
#endif
#ifndef SWIG
	void                                                      printSequenceInDirectory     ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	void                                                      printSequenceInDirectory     ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	void                                                      printSequenceInDirectory     ( std::string path );
#endif

#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory      ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory      ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  fileAndSequenceInDirectory      ( std::string path );
#endif
#ifndef SWIG
	void                                                      printFileAndSequenceInDirectory ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	void                                                      printFileAndSequenceInDirectory ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	void                                                      printFileAndSequenceInDirectory ( std::string path );
#endif
#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory               ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory               ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  folderInDirectory               ( std::string path );
#endif
#ifndef SWIG
	void                                                      printFolderInDirectory          ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	void                                                      printFolderInDirectory          ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	void                                                      printFolderInDirectory          ( std::string path );
#endif


#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory           ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskType mask = eMaskTypeDefault,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory           ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskType mask = eMaskTypeDefault,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  fileObjectInDirectory           ( std::string path );
#endif
#ifndef SWIG
	void                                                      printFileObjectInDirectory      ( const boost::filesystem::path& directory,
												    const sequenceParser::EMaskType mask = eMaskTypeDefault,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	void                                                      printFileObjectInDirectory      ( const boost::filesystem::path& directory,
												    std::vector<std::string>& filters,
												    const sequenceParser::EMaskType mask = eMaskTypeDefault,
												    const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	void                                                      printFileObjectInDirectory      ( std::string path );
#endif
private:
	/**
	 * Detect if the filename is filtered by one of the filter
	 *
	 * @param[in] filename filename need to be check if it filtered
	 * @param[in] filters vector of filters
	 *
	 * @return return true if the filename is filtered by filter(s)
	 */
	bool isNotFilter( std::string filename, std::vector<std::string>& filters );

	/**
	 * @brief Create a sequence
	 * @param[in] id: the sequence identifier
	 * @param[inout] nums: the list of sequence of number inside each filename
	 * @warning We modify nums in place, because we don't use it after
	 *          so there is no reason to create a copy.
	 * @return a sequence object with all informations
	 */
	std::list<sequenceParser::Sequence> buildSequence( const boost::filesystem::path& directory, const sequenceParser::FileStrings& id, std::list<sequenceParser::FileNumbers>& nums, const sequenceParser::EMaskOptions& desc );

	/**
	 * @brief Construct id and nums from a filename.
	 * @param[in] str: the string to process (filename)
	 * @param[out] id: list of strings
	 * @param[out] nums: list of integers
	 * @return number of decteted numbers
	 */
	std::size_t seqConstruct( const std::string& str, FileStrings& id, FileNumbers& nums );
};

}
#endif
