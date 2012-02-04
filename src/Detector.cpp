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
	//std::cout << "$$ " << filter << std::endl;
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

std::vector<boost::shared_ptr<File> > Detector::fileInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileInDirectory( directory, filters, desc );
}

std::vector<boost::shared_ptr<File> > Detector::fileInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<boost::shared_ptr<File> > outputFiles;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputFiles;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;

	// temporary variables, should be inside the loop but here for optimization.
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the vector of numbers inside one filename

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
						// append the vector of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::vector<FileNumbers> li;
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
	// add sequences in the output vector
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		if( p.second.size() == 1 )
		{
			std::string filename = p.first[0];
			filename += p.second[0].getString(0);
			filename += p.first[1];
			//std::cout << "FILENAME = " << filename << std::endl;
			boost::shared_ptr<File> file( new File( directory, filename, desc ) );
			outputFiles.push_back( file );
		}
	}

	return outputFiles;
}

std::vector<boost::shared_ptr<Sequence> > Detector::sequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return sequenceInDirectory( directory, filters, desc );
}

std::vector<boost::shared_ptr<Sequence> > Detector::sequenceInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<boost::shared_ptr<Sequence> > outputSequences;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputSequences;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the vector of numbers inside one filename

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
						// append the vector of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::vector<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
			}
		}
	}
	// add sequences in the output vector

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
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

std::vector<boost::shared_ptr<Sequence> > Detector::sequenceFromFilenameList( const std::vector<boost::filesystem::path>& filenames, const EMaskOptions desc )
{
	std::vector<std::string> filters; // @todo as argument !
	std::vector<boost::shared_ptr<Sequence> > outputSequences;

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the vector of numbers inside one filename

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
						// append the vector of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::vector<FileNumbers> li;
						li.push_back( tmpNumberParts );
						sequences.insert( SeqIdMap::value_type( tmpStringParts, li ) );
					}
				}
			}
		}
	}

	boost::filesystem::path directory; ///< @todo filter by directories, etc.

	// add sequences in the output vector
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		//std::cout << "FileStrings: " << p.first << std::endl;
		const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, desc );

		BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
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

std::vector<boost::shared_ptr<FileObject> > Detector::fileAndSequenceInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileAndSequenceInDirectory( directory, filters, desc );
}

std::vector<boost::shared_ptr<FileObject> > Detector::fileAndSequenceInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<boost::shared_ptr<FileObject> > output;
	std::vector<boost::shared_ptr<FileObject> > outputFiles;
	std::vector<boost::shared_ptr<FileObject> > outputSequences;
	std::string tmpDir( dir );

	if( ! detectDirectoryInResearch( tmpDir, filters ) )
	{
		return output;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the vector of numbers inside one filename

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
						// append the vector of numbers
						sequences.at( tmpStringParts ).push_back( tmpNumberParts );
					}
					else
					{
						// create an entry in the map
						std::vector<FileNumbers> li;
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
	// add sequences in the output vector

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		if( p.second.size() == 1 )
		{
			std::string filename = p.first[0];
			filename += p.second[0].getString(0);
			filename += p.first[1];
			//std::cout << "FILENAME = " << filename << std::endl;
			boost::shared_ptr<File> file( new File( directory, filename, desc ) );
			outputFiles.push_back( file );
		}
		else
		{
			const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, desc );

			BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
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
	}

	output.insert( output.begin(), outputFiles.begin(), outputFiles.end() );
	output.insert( output.begin(), outputSequences.begin(), outputSequences.end() );
	return output;
}

std::vector<boost::shared_ptr<Folder> > Detector::folderInDirectory( const std::string& directory, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return folderInDirectory( directory, filters, desc );
}

std::vector<boost::shared_ptr<Folder> > Detector::folderInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<boost::shared_ptr<Folder> > outputFolders;
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

std::vector<boost::shared_ptr<FileObject> > Detector::fileObjectInDirectory( const std::string& directory, const EMaskType mask, const EMaskOptions desc )
{
	std::vector<std::string> filters;
	return fileObjectInDirectory( directory, filters, mask, desc );
}

std::vector<boost::shared_ptr<FileObject> > Detector::fileObjectInDirectory( const std::string& dir, std::vector<std::string>& filters, const EMaskType mask, const EMaskOptions desc )
{
	std::vector<boost::shared_ptr<FileObject> > output;
	std::vector<boost::shared_ptr<FileObject> > outputFolders;
	std::vector<boost::shared_ptr<FileObject> > outputFiles;
	std::vector<boost::shared_ptr<FileObject> > outputSequences;
	std::string tmpDir( dir );

	if( !detectDirectoryInResearch( tmpDir, filters ) )
	{
		return outputFiles;
	}

	const std::vector<boost::regex> reFilters = convertFilterToRegex( filters, desc );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( tmpDir );
	SeqIdMap sequences;
	FileStrings tmpStringParts; // an object uniquely identify a sequence
	FileNumbers tmpNumberParts; // the vector of numbers inside one filename

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
							// append the vector of numbers
							sequences.at( tmpStringParts ).push_back( tmpNumberParts );
						}
						else
						{
							// create an entry in the map
							std::vector<FileNumbers> li;
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
	// add sequences in the output vector

	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		if( p.second.size() == 1 )
		{
			std::string filename = p.first[0];
			filename += p.second[0].getString(0);
			filename += p.first[1];
			//std::cout << "FILENAME = " << filename << std::endl;
			boost::shared_ptr<File> file( new File( directory, filename, desc ) );
			outputFiles.push_back( file );
		}
		else
		{
			const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, desc );

			BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
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
	}

	if( mask & eMaskTypeDirectory )
	{
		output.insert( output.end(), outputFolders.begin(), outputFolders.end() );
	}
	// add files in the output vector
	if( mask & eMaskTypeFile )
	{
		output.insert( output.end(), outputFiles.begin(), outputFiles.end() );
	}
	// add sequences in the output vector
	if( mask & eMaskTypeSequence )
	{
		output.insert( output.end(), outputSequences.begin(), outputSequences.end() );
	}
	return output;
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

Sequence Detector::privateBuildSequence(
		const Sequence& defaultSeq,
		const FileStrings& stringParts,
		const std::vector<FileNumbers>::const_iterator& numberPartsBegin,
		const std::vector<FileNumbers>::const_iterator& numberPartsEnd,
		const std::size_t index,
		const std::size_t padding,
		const bool strictPadding
	) const
{
	const std::size_t len = numberPartsBegin->size();
	Sequence sequence( defaultSeq );

	// fill information in the sequence...
	for( std::size_t i = 0; i < index; ++i )
	{
		sequence._prefix += stringParts[i];
		sequence._prefix += numberPartsBegin->getString( i );
	}
	sequence._prefix += stringParts[index];
	for( std::size_t i = index + 1; i < len; ++i )
	{
		sequence._suffix += stringParts[i];
		sequence._suffix += numberPartsBegin->getString( i );
	}
	sequence._suffix += stringParts[len];

	std::vector<FileNumbers>::const_iterator numberPartsLast = numberPartsEnd;
	--numberPartsLast;

	// standard case, one sequence detected
	sequence._firstTime = numberPartsBegin->getTime( index );
	sequence._lastTime = numberPartsLast->getTime( index );
	sequence._nbFiles = std::distance( numberPartsBegin, numberPartsEnd );

	sequence.extractStep( numberPartsBegin, numberPartsEnd, index );
	//sequence.extractPadding( numberPartsBegin, numberPartsEnd, index );
	sequence._padding = padding;
	sequence._strictPadding = strictPadding;
	//sequence.extractIsStrictPadding( numberPartsBegin, numberPartsEnd, index, sequence._padding );

	return sequence;
}

/**
 *
 * @param result
 * @param numberPartsBegin
 * @param numberPartsEnd
 * @param index
 */
void Detector::privateBuildSequencesAccordingToPadding(
	std::vector<Sequence>& result,
	const Sequence& defaultSeq,
	const FileStrings& stringParts,
	const std::vector<FileNumbers>::iterator& numberPartsBegin,
	const std::vector<FileNumbers>::iterator numberPartsEnd,
	const int index ) const
{
	std::set<std::size_t> paddings;
	std::set<std::size_t> ambiguousPaddindDigits;
	for( std::vector<FileNumbers>::const_iterator it = numberPartsBegin;
	     it != numberPartsEnd;
		 ++it )
	{
		const std::size_t padding  = it->getPadding(index);
		const std::size_t nbDigits = it->getNbDigits(index);

		paddings.insert( padding );

		if( padding == 0 )
		{
			ambiguousPaddindDigits.insert( nbDigits );
		}
	}

	if( paddings.size() == 1 )
	{
		// standard case: only one padding used in the sequence!
		const std::size_t p = *paddings.begin();
		// simple sort
		std::sort( numberPartsBegin, numberPartsEnd, FileNumbers::SortByNumber() );
		result.push_back( privateBuildSequence( defaultSeq, stringParts, numberPartsBegin, numberPartsEnd, index, p, (p!=0) ) );
		return;
	}

	bool onlyConsiderPadding = false;
	if( paddings.find( 0 ) == paddings.end() )
	{
		// No element without padding.
		// All parts are prefixed by 0, only strict padding,
		// so we can sort by padding without ambiguity
		onlyConsiderPadding = true;
	}
	else
	{
		// We have a mix of padding and no padding.
		// It may be the same number of digits (strict or ambiguous padding: [09, 10]).
		// Some ambiguous cases:
		//	--------------------------------------------------------------------------------
		//	|     sort by padding      |     sort by padding     |     sort by digits      |
		//	--------------------------------------------------------------------------------
		//	|  number  padding digits  |  number padding digits  |  number padding digits  |
		//	|  1       0       1       |  100    0       3       |  1      0       1       |
		//	|  5       0       1       |  102    0       3       |  5      0       1       |
		//	|  10      0       2       |                         |  10     0       2       |
		//	|  100     0       3       |  001    3       3       |                         |
		//	|  102     0       3       |  002    3       3       |  001    3       3       |
		//	|  1000    0       4       |  099    3       3       |  002    3       3       |
		//	|                          |                         |  099    3       3       |
		//	|  001     3       3       |  0001   4       4       |  100    0       3       |
		//	|  002     3       3       |  0123   4       4       |  102    0       3       |
		//	|  099     3       3       |  1234   4       4       |                         |
		//	|                          |                         |  0001   4       4       |
		//	|  0001    4       4       |                         |  0123   4       4       |
		//	|  0123    4       4       |                         |  1234   4       4       |
		//	|  1234    4       4       |                         |                         |
		//	|                          |                         |  10000  0       5       |
		//	--------------------------------------------------------------------------------
		//	|                          |  The sequence without   |   One sequence without  |
		//	|   One sequence without   |  padding can be merge   | padding, should use a   |
		//	|        padding           |   in sequence with      |     sort by padding     |
		//	|                          |        padding 3        |                         |
		//	--------------------------------------------------------------------------------
		//	|          YES             |   NO : sort by digits   |  NO : sort by padding   |
		//	--------------------------------------------------------------------------------
		onlyConsiderPadding = false;
		BOOST_FOREACH( const std::size_t dig, ambiguousPaddindDigits )
		{
			if( paddings.find( dig ) == paddings.end() )
			{
				// if one digits from ambiguous digits doesn't correspond to
				// a padding... we keep the whole sequence without padding.
				onlyConsiderPadding = true;
				break;
			}
		}
	}

	if( onlyConsiderPadding )
	{
		//std::cout << "Detector onlyConsiderPadding: " << __LINE__ << std::endl;
		// sort by padding
		std::sort( numberPartsBegin, numberPartsEnd, FileNumbers::SortByPadding() );
		// split when the padding changed
		std::vector<FileNumbers>::const_iterator start = numberPartsBegin;
		for( std::vector<FileNumbers>::const_iterator it = boost::next(start); it != numberPartsEnd; ++it )
		{
			if( start->getPadding(index) != it->getPadding(index) )
			{
				const std::size_t p = start->getPadding(index);
				result.push_back( privateBuildSequence( defaultSeq, stringParts, start, it, index, p, (p!=0) ) );
				start = it;
			}
		}
		const std::size_t p = start->getPadding(index);
		result.push_back( privateBuildSequence( defaultSeq, stringParts, start, numberPartsEnd, index, p, (p!=0) ) );
		return;
	}
	else
	{
		//std::cout << "Detector onlyConsiderDigits: " << __LINE__ << std::endl;
		// sort by digits
		std::sort( numberPartsBegin, numberPartsEnd, FileNumbers::SortByDigit() );
		// split when the number of digits changed
		std::vector<FileNumbers>::const_iterator start = numberPartsBegin;
		for( std::vector<FileNumbers>::const_iterator it = boost::next(numberPartsBegin); it != numberPartsEnd; ++it )
		{
			if( start->getNbDigits(index) != it->getNbDigits(index) )
			{
				const std::size_t p = boost::prior(it)->getPadding(index);
				const std::size_t pStart = start->getPadding(index);
				result.push_back( privateBuildSequence( defaultSeq, stringParts, start, it, index, pStart, (p!=pStart) ) );
				start = it;
			}
		}
		const std::size_t p = boost::prior(numberPartsEnd)->getPadding(index);
		const std::size_t pStart = start->getPadding(index);
		result.push_back( privateBuildSequence( defaultSeq, stringParts, start, numberPartsEnd, index, pStart, (p!=pStart) ) );
		return;
	}
}


bool getVaryingNumber( std::ssize_t& index, const FileNumbers& a, const FileNumbers& b )
{
	BOOST_ASSERT( a.size() == b.size() );
	bool foundOne = false;
	for( std::size_t i = 0; i < a.size(); ++i )
	{
		if( a.getString(i) != b.getString(i) )
		{
			if( foundOne )
			{
				index = -1;
				return false; // more than one element founded
			}
			foundOne = true;
			index = i;
		}
	}
	if( !foundOne )
		index = -1;
	return foundOne; // we found one varying index
}

std::vector<Sequence> Detector::buildSequences( const boost::filesystem::path& directory, const FileStrings& stringParts, std::vector<FileNumbers>& numberParts, const EMaskOptions& desc )
{
	Sequence defaultSeq( directory, desc );
	BOOST_ASSERT( numberParts.size() > 0 );
	// assert all FileNumbers have the same size...
	BOOST_ASSERT( numberParts.front().size() == numberParts.back().size() );
	const std::size_t len = numberParts.front().size();
	std::vector<Sequence> result;
	
	if( numberParts.size() <= 1 )
	{
		privateBuildSequencesAccordingToPadding( result, defaultSeq, stringParts, numberParts.begin(), numberParts.end(), len-1 );
		return result;
	}
	
	// detect which part is the sequence number
	// for the moment, accept only one sequence
	// but we can easily support multi-sequences
	std::vector<std::size_t> allIndex; // vector of indices (with 0 < index < len) with value changes
	for( std::size_t i = 0; i < len; ++i )
	{
		const std::string t = numberParts.front().getString( i );

		BOOST_FOREACH( const FileNumbers& sn, numberParts )
		{
			if( sn.getString( i ) != t )
			{
				allIndex.push_back( i );
				break;
			}
		}
	}
	
	//std::cout << "allIndex.size(): " << allIndex.size() << std::endl;
	
	if( allIndex.size() == 1 )
	{
		// if it's a simple sequence, but may be mix multiple paddings
		privateBuildSequencesAccordingToPadding( result, defaultSeq, stringParts, numberParts.begin(), numberParts.end(), allIndex.front() );
		return result;
	}
	
	// it's a multi-sequence
	
	// ambiguous example
	// 1 2 3
	// 1 3 3
	// 1 4 3
	// 1 5 3 // could go in both sequences
	//// split here
	// 1 5 4
	// 1 5 5
	// 1 5 6
	// 1 5 7
	
	std::sort( numberParts.begin(), numberParts.end(), FileNumbers::SortByPadding() );

	std::vector<FileNumbers>::iterator start = numberParts.begin();
	std::vector<FileNumbers>::iterator it = boost::next(start);
	std::vector<FileNumbers>::iterator itEnd = numberParts.end();
	std::ssize_t previousIndex = -1;
	std::ssize_t index = -1;
	bool split = false;
	
	for( ; it != itEnd; ++it )
	{
		//std::cout << "________________________________________" <<  std::endl;
		//std::cout << "start: " << *start <<  std::endl;
		//std::cout << "it: " << *it <<  std::endl;
		if( getVaryingNumber( index, *start, *it ) )
		{
			if( previousIndex != -1 && // we previously have a sequence and
			    index != previousIndex ) // the index is not the same than previous: split!
			{
				split = true;
			}
//			else
//			{
//				// we don't have a sequence before, there is now one varying number,
//				// so it's the next sequence
//			}
		}
		else
		{
			// more than one varying number: split in all cases!
			split = true;
			// if no sequence before... the file is alone...
			// Set the number as the last number.
			if( previousIndex == -1 )
				previousIndex = start->size() - 1;
		}
		if( split )
		{
			privateBuildSequencesAccordingToPadding( result, defaultSeq, stringParts, start, it, previousIndex );
			split = false;
			index = -1;
			start = it;
		}
		previousIndex = index;
	}
	if( previousIndex == -1 )
		previousIndex = start->size() - 1;
	privateBuildSequencesAccordingToPadding( result, defaultSeq, stringParts, start, it, previousIndex );
	
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
