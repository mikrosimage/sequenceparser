#include "filesystem.hpp"
#include "Sequence.hpp"
#include "Folder.hpp"
#include "File.hpp"
#include "utils.hpp"

#include "detail/analyze.hpp"
#include "detail/FileNumbers.hpp"
#include "detail/FileStrings.hpp"
#include "commonDefinitions.hpp"

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


std::vector<Item> browse(
	const std::string& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
	std::vector<Item> output;
	std::string tmpDir( dir );
	std::string filename;

	if( ! detectDirectoryInResearch( tmpDir, filters, filename ) )
		return output;

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

		if( isNotFilter( iter->path(), reFilters, filename, desc ) )
		{
			// it's a file or a file of a sequence
			if( filenameIsNotFilter( iter->path().filename().string(), reFilters ) ) // filtering of entries with filters strings
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
					EType type = bfs::is_directory(iter->path()) ? eTypeFolder : eTypeFile;
					output.push_back( Item( type, iter->path().filename().string(), directory.string() ) );
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
			output.push_back( Item( eTypeFile, filename, directory.string() ) );
		}
		else
		{
			const std::vector<Sequence> ss = buildSequences( directory, p.first, p.second, desc );

			BOOST_FOREACH( const std::vector<Sequence>::value_type & s, ss )
			{
				// don't detect sequence of directories
				if( bfs::is_directory( s.getAbsoluteFirstFilename() ) )
				{
					// TODO: declare folders individually
				}
				else
				{
					if( s.getNbFiles() == 1 ) // if it's a sequence of 1 file, it isn't a sequence but only a file
					{
						output.push_back( Item( eTypeFile, s.getFirstFilename(), directory.string() ) );
					}
					else
					{
						output.push_back( Item( Sequence( directory, s, desc ), directory.string() ) );
					}
				}
			}
		}
	}
	return output;
}


}
