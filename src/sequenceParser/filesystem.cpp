#include "filesystem.hpp"

#include "utils.hpp"

#include "detail/analyze.hpp"
#include "detail/FileNumbers.hpp"
#include "detail/FileStrings.hpp"

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <set>


namespace sequenceParser {

using detail::FileNumbers;
using detail::FileStrings;
using detail::SeqIdHash;
namespace bfs = boost::filesystem;


bfs::path getDirectoryFromPath( const bfs::path& p )
{
	// if it's not a directory, use the parent directory of the file
	bfs::path directory = p.parent_path();
	if( directory.empty() ) // relative path
	{
		directory = bfs::current_path();
	}
	return directory;
}

bool browseSequence( Sequence& outSequence, const std::string& pattern, const EPattern accept )
{
	outSequence.clear();
	bfs::path directory = getDirectoryFromPath( pattern );

	if( !outSequence.initFromPattern( bfs::path( pattern ).filename().string(), accept ) )
		return false; // not recognized as a pattern, maybe a still file

	if( !bfs::exists( directory ) )
		return false; // an empty sequence

	std::vector<std::string> allTimesStr;
	std::vector<Time> allTimes;
	bfs::directory_iterator itEnd;

	for( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// we don't make this check, which can take long time on big sequences (>1000 files)
		// depending on your filesystem, we may need to do a stat() for each file
		// if( bfs::is_directory( iter->status() ) )
		// continue; // skip directories
		Time time;
		std::string timeStr;

		// if the file is inside the sequence
		if( outSequence.isIn( iter->path().filename().string(), time, timeStr ) )
		{
			// create a big vector of all times in our sequence
			allTimesStr.push_back( timeStr );
			allTimes.push_back( time );
		}
	}
	if( allTimes.size() < 2 )
	{
		if( allTimes.size() == 1 )
		{
			outSequence._ranges.push_back(FrameRange(allTimes.front()));
		}
		//std::cout << "empty => " <<  _firstTime << " > " << _lastTime << " : " << _nbFiles << std::endl;
		return true; // an empty sequence
	}
	std::sort( allTimes.begin(), allTimes.end() );
	outSequence._ranges = extractFrameRanges( allTimes );
	return true; // a real file sequence
}

bool isConsideredAsSingleFile( const Sequence& s, const EDetection detectOptions )
{
	return (detectOptions & eDetectionSequenceNeedAtLeastTwoFiles) && (s.getNbFiles() == 1);
}

std::vector<Item> browse(
		const bfs::path& dir,
		const EDetection detectOptions,
		const std::vector<std::string>& filters )
{
	std::vector<Item> output;
	std::string tmpDir( dir.string() );
	std::vector<std::string> tmpFilters( filters );
	std::string filename;

	if( ! detectDirectoryInResearch( tmpDir, tmpFilters, filename ) )
		return output;

	const std::vector<boost::regex> reFilters = convertFilterToRegex( tmpFilters, detectOptions );

	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::vector<FileNumbers>, SeqIdHash> SeqIdMap;
	bfs::path directory( dir );
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

		if( ! filepathRespectsAllFilters( iter->path(), reFilters, filename, detectOptions ) )
			continue;

		// if at least one number detected
		if( decomposeFilename( iter->path().filename().string(), tmpStringParts, tmpNumberParts, detectOptions ) )
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
			output.push_back( Item( getTypeFromPath( iter->path() ), iter->path() ) );
		}
	}

	// add sequences in the output vector
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, detectOptions );

		BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
		{
			if( bfs::is_directory( directory / s.getFirstFilename() ) )
			{
				// It's a sequence of directories, so it's not a sequence.
				BOOST_FOREACH( Time t, s.getFramesIterable() )
				{
					bfs::path folderPath = directory / s.getFilenameAt(t);
					output.push_back( Item( getTypeFromPath(folderPath), folderPath ) );
				}
			}
			else
			{
				// if it's a sequence of 1 file, it could be considered as a sequence or as a single file
				if( isConsideredAsSingleFile( s, detectOptions ) )
				{

					output.push_back( Item( getTypeFromPath( directory / s.getFirstFilename() ), directory / s.getFirstFilename() ) );
				}
				else
				{
					// if it's a sequence with holes, it could be split in several sequences depending on the detect options
					if( (detectOptions & eDetectionSequenceWithoutHoles) && (s.getFrameRanges().size() > 1) )
					{
						BOOST_FOREACH( FrameRange f, s.getFrameRanges() )
						{
							const Sequence sequenceWithoutHoles( s.getPrefix(), s.getFixedPadding(), s.getMaxPadding(), s.getSuffix(), f.first, f.last, f.step );
							if( isConsideredAsSingleFile( sequenceWithoutHoles, detectOptions ) )
							{
								output.push_back( Item( getTypeFromPath( directory / sequenceWithoutHoles.getFirstFilename() ), directory / sequenceWithoutHoles.getFirstFilename() ) );
							}
							else
							{
								output.push_back( Item( Sequence( directory, sequenceWithoutHoles ), directory ) );
							}
						}
					}
					else
					{
						output.push_back( Item( Sequence( directory, s ), directory ) );
					}
				}
			}
		}
	}
	return output;
}


}
