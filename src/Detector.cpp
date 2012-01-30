#include "Detector.hpp"
#include "Sequence.hpp"
#include "Folder.hpp"
#include "File.hpp"

#include "detail/FileNumbers.hpp"
#include "detail/FileStrings.hpp"
#include "commonDefinitions.hpp"

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <set>


namespace sequenceParser {

using detail::FileNumbers;
using detail::FileStrings;
namespace bfs = boost::filesystem;

// NOTE How we can replace this with a wrapper?
// Like boost::function, boost::bind,...

struct SeqIdHash : std::unary_function<FileStrings, std::size_t>
{
	std::size_t operator()(const FileStrings & p ) const
	{
		return p.getHash();
	}
};


boost::regex convertFilterToRegex( std::string filter, const EMaskOptions desc )
{
	boost::cmatch match;
	boost::regex expression( "(.*[%])([0-9]{2})([d].*)" ); // match to pattern like : %04d
	if( boost::regex_match( filter.c_str(), match, expression ) )
	{
		std::string matched = match[1].second;
		matched.erase( 2 , matched.size()-2); // keep only numbers
		const int patternWidth = boost::lexical_cast<int>( matched );
		std::string replacing( patternWidth, '#' );
		filter = boost::regex_replace( filter, boost::regex( "\\%\\d{1,2}d" ), replacing );
	}

	// for detect sequence based on a single file
	if( ( desc & eMaskOptionsSequenceBasedOnFilename ) )
		filter = boost::regex_replace( filter, boost::regex( "\\d" ), "[0-9]" );

	filter = boost::regex_replace( filter, boost::regex( "\\*" ), "(.*)" );
	filter = boost::regex_replace( filter, boost::regex( "\\?" ), "(.)" );
	if( desc & eMaskOptionsNegativeIndexes )
	{
		filter = boost::regex_replace( filter, boost::regex( "\\@" ), "[\\-\\+]?[0-9]+" ); // one @ correspond to one or more digits
		filter = boost::regex_replace( filter, boost::regex( "\\#" ), "[\\-\\+]?[0-9]" ); // each # in pattern correspond to a digit
	}
	else
	{
		filter = boost::regex_replace( filter, boost::regex( "\\@" ), "[0-9]+" ); // one @ correspond to one or more digits
		filter = boost::regex_replace( filter, boost::regex( "\\#" ), "[0-9]" ); // each # in pattern correspond to a digit
	}

	return boost::regex( filter );
}

std::vector<boost::regex> convertFilterToRegex( const std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<boost::regex> res;
	BOOST_FOREACH( const std::string& filter, filters )
	{
		res.push_back( convertFilterToRegex( filter, desc ) );
	}
	return res;
}

/**
 * Detect if the filename is filtered by one of the filter
 *
 * @param[in] filename filename need to be check if it filtered
 * @param[in] filters vector of filters
 * @param[in] desc enable research options (Cf. EMaskOptions in commonDefinitions.hpp )
 *
 * @return return true if the filename is filtered by filter(s)
 */
bool isNotFilter( const std::string& filename, const std::vector<boost::regex>& filters )
{
	if( filters.size() == 0 )
		return true;

	BOOST_FOREACH( const boost::regex& filter, filters )
	{
		if( boost::regex_match( filename, filter ) )
			return true;
	}
	return false;
}



Detector::Detector()
{
}

Detector::~Detector()
{
}

std::list<boost::shared_ptr<File> > Detector::fileInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileInDirectory( directory, filters, desc );
}

std::list<boost::shared_ptr<File> > Detector::fileInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<File> > outputFiles;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputFiles;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;

	// temporary variables, should be inside the loop but here for optimization.
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the list of numbers inside one filename

	// for all files in the directory
	bfs::directory_iterator itEnd;
	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// clear previous infos
		tmpStringParts.clear();
		tmpNumberParts.clear(); // (clear but don't realloc the vector inside)

		if( ( desc & eMaskOptionsDotFile ) || !( iter->path().filename().string()[0] == '.' ) ) // if we ask to show hidden files and if it is hidden
		{
			// it's a file or a file of a sequence
			if( isNotFilter( iter->path().filename().string(), reFilters ) ) // filtering of entries with filters strings
			{
				// if at least one number detected
				if( decomposeFilename( iter->path().filename().string(), tmpStringParts, tmpNumberParts, desc ) )
				{
					// need to construct sequence to detect file with a pattern but with only one image
					const SeqIdMap::iterator it( sequences.find( tmpStringParts ) );
					if( it != sequences.end() ) // is already in map
					{
						// append the list of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::list<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
				else
				{
					if( ! bfs::is_directory( directory / iter->path().filename().string() ) )
					{
						boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
						outputFiles.push_back( f );
					}
				}
			}
		}
	}
	// add sequences in the output list
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( ! bfs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if( s.getNbFiles() == 1 ) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
					boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), desc ) );
					outputFiles.push_back( file );
				}
			}
		}
	}

	return outputFiles;
}

void Detector::printFileInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	printFileInDirectory( directory, filters, desc );
}

void Detector::printFileInDirectory( const std::string& directory, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<File> > outputFiles;
	std::vector<std::string> filenames;

	outputFiles = fileInDirectory( directory, filters, desc );
	for( std::list<boost::shared_ptr<File> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++ )
	{
		boost::shared_ptr<File> f = *it;
		std::cout << *f << std::endl;
	}
}

std::list<boost::shared_ptr<Sequence> > Detector::sequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return sequenceInDirectory( directory, filters, desc );
}

std::list<boost::shared_ptr<Sequence> > Detector::sequenceInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<Sequence> > outputSequences;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputSequences;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the list of numbers inside one filename

	// for all files in the directory
	bfs::directory_iterator itEnd;
	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// clear previous infos
		tmpStringParts.clear();
		tmpNumberParts.clear(); // (clear but don't realloc the vector inside)

		if( !( iter->path().filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
		{
			// it's a file or a file of a sequence
			if( isNotFilter( iter->path().filename().string(), reFilters ) ) // filtering of entries with filters strings
			{
				// if at least one number detected
				if( decomposeFilename( iter->path().filename().string(), tmpStringParts, tmpNumberParts, desc ) )
				{
					const SeqIdMap::iterator it( sequences.find( tmpStringParts ) );
					if( it != sequences.end() ) // is already in map
					{
						// append the list of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::list<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
			}
		}
	}
	// add sequences in the output list

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( !bfs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if( !( s.getNbFiles() == 1 ) ) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
					boost::shared_ptr<Sequence> seq( new Sequence( directory, s, desc ) );
					outputSequences.push_back( seq );
				}
			}
		}
	}

	return outputSequences;
}

std::list<boost::shared_ptr<Sequence> > Detector::sequenceFromFilenameList( const std::vector<boost::filesystem::path>& filenames, const EMaskOptions desc )
{
	std::vector<std::string> filters; // @todo as argument !
	std::list<boost::shared_ptr<Sequence> > outputSequences;

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the list of numbers inside one filename

	// for all files in the directory

	BOOST_FOREACH( const boost::filesystem::path& iter, filenames )
	{
		// clear previous infos
		tmpStringParts.clear();
		tmpNumberParts.clear(); // (clear but don't realloc the vector inside)

		if( !( iter.filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
		{
			// it's a file or a file of a sequence
			if( isNotFilter( iter.filename().string(), reFilters ) ) // filtering of entries with filters strings
			{
				// if at least one number detected
				if( decomposeFilename( iter.filename().string(), tmpStringParts, tmpNumberParts, desc ) )
				{
					const SeqIdMap::iterator it( sequences.find( tmpStringParts ) );
					if( it != sequences.end() ) // is already in map
					{
						// append the list of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::list<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
			}
		}
	}

	boost::filesystem::path directory; ///< @todo filter by directories, etc.

	// add sequences in the output list
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( !bfs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if( !( s.getNbFiles() == 1 ) ) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
					boost::shared_ptr<Sequence> seq( new Sequence( directory, s, desc ) );
					outputSequences.push_back( seq );
				}
			}
		}
	}

	return outputSequences;
}

void Detector::printSequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<Sequence> > outputFiles;

	outputFiles = sequenceInDirectory( directory, eMaskOptionsColor );
	for( std::list<boost::shared_ptr<Sequence> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++ )
	{
		boost::shared_ptr<Sequence> f = *it;
		std::cout << *f << std::endl;
	}
}

std::list<boost::shared_ptr<FileObject> > Detector::fileAndSequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileAndSequenceInDirectory( directory, filters, desc );
}

std::list<boost::shared_ptr<FileObject> > Detector::fileAndSequenceInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<FileObject> > output;
	std::list<boost::shared_ptr<FileObject> > outputFiles;
	std::list<boost::shared_ptr<FileObject> > outputSequences;
	std::string tmpDir( dir );

	if( ! detectDirectoryInResearch( tmpDir, filters ) )
	{
		return output;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the list of numbers inside one filename

	// for all files in the directory
	bfs::directory_iterator itEnd;
	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// clear previous infos
		tmpStringParts.clear();
		tmpNumberParts.clear(); // (clear but don't realloc the vector inside)

		if( !( iter->path().filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
		{
			// it's a file or a file of a sequence
			if( isNotFilter( iter->path().filename().string(), reFilters ) ) // filtering of entries with filters strings
			{
				// if at least one number detected
				if( decomposeFilename( iter->path().filename().string(), tmpStringParts, tmpNumberParts, desc ) )
				{
					const SeqIdMap::iterator it( sequences.find( tmpStringParts ) );
					if( it != sequences.end() ) // is already in map
					{
						// append the list of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::list<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
				else
				{
					boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
					outputFiles.push_back( f );
				}
			}
		}
	}
	// add sequences in the output list

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( !bfs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if( s.getNbFiles() == 1 ) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
					boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), desc ) );
					outputFiles.push_back( file );
				}
				else
				{
					boost::shared_ptr<Sequence> seq( new Sequence( directory, s, desc ) );
					outputSequences.push_back( seq );
				}
			}
		}
	}

	output.merge( outputFiles );
	output.merge( outputSequences );
	return output;
}

void Detector::printFileAndSequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	printFileAndSequenceInDirectory( directory, filters, desc );
}

void Detector::printFileAndSequenceInDirectory( const std::string& directory, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<FileObject> > outputFiles;

	outputFiles = fileAndSequenceInDirectory( directory, filters, desc );
	for( std::list<boost::shared_ptr<FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++ )
	{
		boost::shared_ptr<FileObject> f = *it;
		std::cout << *f << std::endl;
	}
}

std::list<boost::shared_ptr<Folder> > Detector::folderInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return folderInDirectory( directory, filters, desc );
}

std::list<boost::shared_ptr<Folder> > Detector::folderInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<Folder> > outputFolders;
	bfs::path directory;

	if( bfs::exists( dir ) )
	{
		if( bfs::is_directory( dir ) )
		{
			directory = bfs::path( dir );
		}
		else
		{
			return outputFolders;
		}
	}
	else
	{
		return outputFolders;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// for all files in the directory
	bfs::directory_iterator itEnd;
	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		if( !( iter->path().filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
		{
			// detect if is a folder
			if( bfs::is_directory( iter->status() ) )
			{
				boost::shared_ptr<Folder> d( new Folder( directory, iter->path().filename().string(), desc ) );
				outputFolders.push_back( d );
			}
		}
	}
	return outputFolders;
}

void Detector::printFolderInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	printFolderInDirectory( directory, filters, desc );
}

void Detector::printFolderInDirectory( const std::string& directory, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<Folder> > outputFiles;

	outputFiles = folderInDirectory( directory, filters, desc );
	for( std::list<boost::shared_ptr<Folder> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++ )
	{
		boost::shared_ptr<Folder> f = *it;
		std::cout << *f << std::endl;
	}
}

std::list<boost::shared_ptr<FileObject> > Detector::fileObjectInDirectory( const std::string& directory, const EMaskType mask, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileObjectInDirectory( directory, filters, mask, desc );
}

std::list<boost::shared_ptr<FileObject> > Detector::fileObjectInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskType mask, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<FileObject> > output;
	std::list<boost::shared_ptr<FileObject> > outputFolders;
	std::list<boost::shared_ptr<FileObject> > outputFiles;
	std::list<boost::shared_ptr<FileObject> > outputSequences;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputFiles;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the list of numbers inside one filename

	// for all files in the directory
	bfs::directory_iterator itEnd;
	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// clear previous infos
		tmpStringParts.clear();
		tmpNumberParts.clear(); // (clear but don't realloc the vector inside)

		if( !( iter->path().filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
		{
			// @todo: no check that needs to ask the status of the file here (like asking if it's a folder).
			// detect if is a folder
			if( bfs::is_directory( iter->status() ) )
			{
				boost::shared_ptr<Folder> d( new Folder( directory, iter->path().filename().string(), desc ) );
				outputFolders.push_back( d );
			}
			else // it's a file or a file of a sequence
			{
				if( isNotFilter( iter->path().filename().string(), reFilters ) ) // filtering of entries with filters strings
				{
					// if at least one number detected
					if( decomposeFilename( iter->path().filename().string(), tmpStringParts, tmpNumberParts, desc ) )
					{
						const SeqIdMap::iterator it( sequences.find( tmpStringParts ) );
						if( it != sequences.end() ) // is already in map
						{
							// append the list of numbers
							sequences.at( tmpStringParts ).push_back( tmpNumberParts );
						}
						else
						{
							// create an entry in the map
							std::list<FileNumbers> li;
							li.push_back( tmpNumberParts );
							sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
						}
					}
					else
					{
						boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
						outputFiles.push_back( f );
					}
				}
			}
		}
	}
	// add sequences in the output list

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( !bfs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if( s.getNbFiles() == 1 ) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
					boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), desc ) );
					outputFiles.push_back( file );
				}
				else
				{
					boost::shared_ptr<Sequence> seq( new Sequence( directory, s, desc ) );
					outputSequences.push_back( seq );
				}
			}
			// else
			// {
			// @todo loop to declare direcotries !
			// }
		}
	}

	if( mask & eMaskTypeDirectory )
	{
		output.merge( outputFolders );
	}
	// add files in the output list
	if( mask & eMaskTypeFile )
	{
		output.merge( outputFiles );
	}
	// add sequences in the output list
	if( mask & eMaskTypeSequence )
	{
		output.merge( outputSequences );
	}
	return output;
}

void Detector::printFileObjectInDirectory( const std::string& directory, const EMaskType mask, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	printFileObjectInDirectory( directory, filters, mask, desc );
}

void Detector::printFileObjectInDirectory( const std::string& directory, std::vector<std::string>& filters, const EMaskType mask, const EMaskOptions desc )
{
	std::list<boost::shared_ptr<FileObject> > outputFiles;

	outputFiles = fileObjectInDirectory( directory, filters, mask, desc );
	for( std::list<boost::shared_ptr<FileObject> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++ )
	{
		boost::shared_ptr<FileObject> f = *it;
		std::cout << *f << std::endl;
	}
}

bool Detector::detectDirectoryInResearch( std::string& researchPath, std::vector<std::string>& filters )
{
	if( bfs::exists( researchPath ) )
	{
		if( !bfs::is_directory( researchPath ) )
		{
			// the researchPath is an existing file, we search into the parent directory with filtering these filename
			// warning: can find a sequence based on a filename
			bfs::path tmpPath( researchPath );
			filters.push_back( tmpPath.filename().string() );
			if( tmpPath.has_parent_path() )
				researchPath = tmpPath.parent_path().string();

			else
				researchPath = "./";
		}
		// else
		// the researchPath is a directory, we search into the directory without filtering
	}
	else
	{
		bfs::path tmpPath( researchPath );
		if( !tmpPath.has_parent_path() )
		{
			filters.push_back( researchPath );
			researchPath = ".";
			return true;
		}
		if( !bfs::exists( tmpPath.parent_path() ) )
		{
			// researchPath and it parent don't exists, could not find file/sequence/folder
			return false;
		}
		// the researchPath is not a directory, but it parent is a directory
		// we search in this parent directory, with the filtering pattern
		filters.push_back( tmpPath.filename().string() );
		researchPath = tmpPath.parent_path().string();
	}
	return true;
}

std::list<Sequence> Detector::buildSequence( const boost::filesystem::path& directory, const FileStrings& stringParts, std::list<FileNumbers>& numberParts, const EMaskOptions& desc )
{
	numberParts.sort();
	BOOST_ASSERT( numberParts.size() > 0 );
	// assert all FileNumbers have the same size...
	BOOST_ASSERT( numberParts.front().size() == numberParts.back().size() );
	const std::size_t len = numberParts.front().size();

	// detect which part is the sequence number
	// for the moment, accept only one sequence
	// but we can easily support multi-sequences
	std::vector<std::size_t> allIndex; // list of indices (with 0 < index < len) with value changes
	for( std::size_t i = 0; i < len; ++i )
	{
		const Time t = numberParts.front().getTime( i );

		BOOST_FOREACH( const FileNumbers& sn, numberParts )
		{
			if( sn.getTime( i ) != t )
			{
				allIndex.push_back( i );
				break;
			}
		}
	}
	std::size_t idChangeBegin = 0;
	std::size_t idChangeEnd = 0;
	if( allIndex.size() == 0 )
	{
		idChangeBegin = idChangeEnd = len - 1;
	}
	else
	{
		idChangeBegin = allIndex.front();
		idChangeEnd = allIndex.back();
	}
	Sequence seqCommon( directory, desc );
	// fill information in the sequence...
	for( std::size_t i = 0; i < idChangeBegin; ++i )
	{
		seqCommon._prefix += stringParts[i];
		seqCommon._prefix += numberParts.front().getString( i );
	}
	seqCommon._prefix += stringParts[idChangeBegin];
	for( std::size_t i = idChangeEnd + 1; i < len; ++i )
	{
		seqCommon._suffix += stringParts[i];
		seqCommon._suffix += numberParts.front().getString( i );
	}
	seqCommon._suffix += stringParts[len];
	std::list<Sequence> result;
	if( allIndex.size() <= 1 )
	{
		// standard case, one sequence detected
		seqCommon._firstTime = numberParts.front().getTime( idChangeEnd );
		seqCommon._lastTime = numberParts.back().getTime( idChangeEnd );
		seqCommon._nbFiles = numberParts.size();

		seqCommon.extractStep( numberParts, idChangeEnd );
		seqCommon.extractPadding( numberParts, idChangeEnd );
		seqCommon.extractIsStrictPadding( numberParts, idChangeEnd, seqCommon._padding );
		result.push_back( seqCommon );
		return result;
	}
	// it's a multi-sequence...
	const FileNumbers* previous = &numberParts.front();
	Sequence s = seqCommon;
	s._prefix += previous->getString( idChangeBegin );
	for( std::size_t i = idChangeBegin + 1; i < idChangeEnd; ++i )
	{
		s._prefix += stringParts[i];
		s._prefix += previous->getString( i );
	}
	s._prefix += stringParts[idChangeEnd];
	result.push_back( s );
	std::list<Time> times;
	std::list<std::string> timesStr;
	std::size_t iCurrent = 0;

	BOOST_FOREACH( const FileNumbers& sn, numberParts )
	{
		if( !sn.rangeEquals( *previous, idChangeBegin, idChangeEnd ) )
		{
			// update the last added sequence
			result.back()._nbFiles = times.size();
			result.back()._firstTime = times.front();
			result.back()._lastTime = times.back();
			result.back().extractStep( times );
			result.back().extractPadding( timesStr );
			result.back().extractIsStrictPadding( timesStr, result.back()._padding );
			times.clear();

			// create a new sequence initilized with "sn" values
			s = seqCommon;
			s._prefix += sn.getString( idChangeBegin );
			for( std::size_t i = idChangeBegin + 1; i < idChangeEnd; ++i )
			{
				s._prefix += stringParts[i];
				s._prefix += sn.getString( i );
			}
			s._prefix += stringParts[idChangeEnd];
			s._padding = sn.getPadding( idChangeEnd );
			result.push_back( s );
			previous = &sn;
		}
		times.push_back( sn.getTime( idChangeEnd ) );
		timesStr.push_back( sn.getString( idChangeEnd ) );
		++iCurrent;
	}

	// update the last added sequence
	result.back()._nbFiles = times.size();
	result.back()._firstTime = times.front();
	result.back()._lastTime = times.back();

	result.back().extractStep( times );
	result.back().extractPadding( timesStr );
	result.back().extractIsStrictPadding( timesStr, result.back()._padding );

	return result;
}

std::size_t Detector::decomposeFilename( const std::string& filename, FileStrings& stringParts, FileNumbers& numberParts, const EMaskOptions& options )
{
	static const std::size_t max = std::numeric_limits<std::size_t>::digits10;
	std::string regex;
	if( options & eMaskOptionsNegativeIndexes )
	{
		regex = "[\\+\\-]?+\\d{1," + boost::lexical_cast<std::string>( max ) + "}";
	}
	else
	{
		regex = "\\d{1," + boost::lexical_cast<std::string>( max ) + "}";
	}
	const boost::regex re( regex );
	static const int subs[] = { -1, 0 }; // get before match and current match
	boost::sregex_token_iterator m( filename.begin(), filename.end(), re, subs );
	boost::sregex_token_iterator end;

//	std::cout << "________________________________________" << std::endl;
//	std::cout << "filename: " << filename << std::endl;
//	std::cout << "regex: " << regex << std::endl;
	while( m != end )
	{
		// begin with string id, can be an empty string if str begins with a number
//		std::cout << "stringPart: " << *m << std::endl;
		stringParts.getId().push_back( *m++ );
		if( m != end ) // if end with a string and not a number
		{
//			std::cout << "numberPart: " << *m << std::endl;
			numberParts.push_back( *m++ );
		}
	}
	if( stringParts.getId().size() == numberParts.size() )
	{
		stringParts.getId().push_back( "" ); // we end with an empty string
	}
	//std::cout << numberParts.size() << std::endl;
	return numberParts.size();
}

}
