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
	  * fileInDirectory methods
	  *
	  **/
#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory              ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileInDirectory( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory              ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  fileInDirectory              ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::File> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::File> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::File> f = *it;
			filenames.push_back( f->getAbsoluteFilename().c_str() );
		}
		return filenames;
	}
#endif
	void                                                      printFileInDirectory         ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::File> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileInDirectory ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::File> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::File> f = *it;
			std::cout << *f << std::endl;
		}
	}


#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory          ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return sequenceInDirectory( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory          ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  sequenceInDirectory          ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Sequence> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = sequenceInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::Sequence> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Sequence> f = *it;
			filenames.push_back( f->getAbsoluteStandardPattern().c_str() );
		}
		return filenames;
	}
#endif
	void                                                      printSequenceInDirectory     ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Sequence> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = sequenceInDirectory ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::Sequence> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Sequence> f = *it;
			std::cout << *f << std::endl;
		}
	}


#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory   ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileAndSequenceInDirectory( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory   ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  fileAndSequenceInDirectory   ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::FileObject> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileAndSequenceInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			//boost::shared_ptr<sequenceParser::FileObject> f = *it;
			//filenames.push_back( f->getAbsoluteStandardPattern().c_str() );
		}
		return filenames;
	}
#endif
	void                                                      printFileAndSequenceInDirectory ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::FileObject> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileAndSequenceInDirectory ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::FileObject> f = *it;
			std::cout << *f << std::endl;
		}
	}

#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory            ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return folderInDirectory( directory, filters, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory            ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );
#else
	std::vector<std::string>                                  folderInDirectory            ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Folder> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = folderInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::Folder> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Folder> f = *it;
			std::vector<boost::filesystem::path> pathDir = f->getFiles ();
			filenames.push_back( pathDir.at(0).c_str() );
		}
		return filenames;
	}
#endif
	void                                                      printFolderInDirectory       ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Folder> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = folderInDirectory ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::Folder> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Folder> f = *it;
			std::cout << *f << std::endl;
		}
	}

#ifndef SWIG
	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory        ( const boost::filesystem::path& directory,
												 const sequenceParser::EMaskType mask = eMaskTypeDefault,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault )
	{
		std::vector<std::string> filters;
		return fileObjectInDirectory ( directory, filters, mask, desc );
	}

	std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory        ( const boost::filesystem::path& directory,
												 std::vector<std::string>& filters,
												 const sequenceParser::EMaskType mask = eMaskTypeDefault,
												 const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

	std::vector<std::string>                                  fileObjectInDirectory        ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::FileObject> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileObjectInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::FileObject> f = *it;
			//std::vector<boost::filesystem::path> pathDir = f->getFiles ();
			//filenames.push_back( pathDir.at(0).c_str() );
		}
		return filenames;
	}
#endif
	void                                                      printFileObjectInDirectory  ( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::FileObject> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = fileObjectInDirectory ( p );
		for (std::list<boost::shared_ptr<sequenceParser::FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::FileObject> f = *it;
			std::cout << *f << std::endl;
		}
	}

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
