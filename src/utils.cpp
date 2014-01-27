#include "detector.hpp"
#include "Sequence.hpp"
#include "Folder.hpp"
#include "File.hpp"

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
namespace bfs = boost::filesystem;

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

bool filenameIsNotFilter( const std::string& filename, const std::vector<boost::regex>& filters )
{
	if( filters.size() == 0 )
		return true;

	BOOST_FOREACH( const boost::regex& filter, filters )
	{
		if( boost::regex_match( filename, filter ) )
		{
			return true;
		}
	}
	return false;
}

bool isNotFilter( const bfs::path& inputPath, const std::vector<boost::regex>& filters, const std::string& filename, const EMaskOptions desc )
{
	if( !( inputPath.filename().string()[0] == '.' ) || ( desc & eMaskOptionsDotFile ) ) // if we ask to show hidden files and if it is hidden
	{
		if( filenameIsNotFilter( inputPath.filename().string(), filters ) ) // filtering of entries with filters strings
		{
			if( filename.size() )
			{
				if( filename == inputPath.string() )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

}
