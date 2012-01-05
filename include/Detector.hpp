#ifndef _DETECTOR_HPP_
#define _DETECTOR_HPP_

#include <FileObject.hpp>
#include <FileStrings.hpp>
#include <FileNumbers.hpp>
#include <Sequence.hpp>
#include <File.hpp>
#include <Folder.hpp>
#include <boost/lexical_cast.hpp>
#include <iomanip>

namespace sequenceParser {

class Detector
{
public :
	Detector();
	~Detector();

	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDir              ( const boost::filesystem::path& directory,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileInDir( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDir              ( const boost::filesystem::path& directory,
											   std::vector<std::string>& filters,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDir          ( const boost::filesystem::path& directory,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return sequenceInDir( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDir          ( const boost::filesystem::path& directory,
											   std::vector<std::string>& filters,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDir   ( const boost::filesystem::path& directory,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileAndSequenceInDir( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDir   ( const boost::filesystem::path& directory,
											   std::vector<std::string>& filters,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDir            ( const boost::filesystem::path& directory,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return folderInDir( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDir            ( const boost::filesystem::path& directory,
											   std::vector<std::string>& filters,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectsInDir       ( const boost::filesystem::path& directory,
											   const sequenceParser::EMaskType mask = eMaskTypeDefault,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileObjectsInDir( directory, filters, mask, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectsInDir       ( const boost::filesystem::path& directory,
											   std::vector<std::string>& filters,
											   const sequenceParser::EMaskType mask = eMaskTypeDefault,
											   const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

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
