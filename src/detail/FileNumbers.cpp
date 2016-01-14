#include "FileNumbers.hpp"

namespace sequenceParser {
namespace detail {


bool FileNumbers::SortByNumber::operator()( const FileNumbers& a, const FileNumbers& b ) const
{
	// can't have multiple size, if multiple size they must have a
	// different SeqId
	BOOST_ASSERT( a._numbers.size() == b._numbers.size() );
	for( FileNumbers::Vec::const_iterator i = a._numbers.begin(), iEnd = a._numbers.end(), vi = b._numbers.begin(); i != iEnd; ++i, ++vi )
	{
		if( i->first < vi->first )
			return true;
		else if( i->first > vi->first )
			return false;
	}
	return false; // equals
}

bool FileNumbers::SortByPadding::operator()( const FileNumbers& a, const FileNumbers& b ) const
{
	// can't have multiple size, if multiple size they must have a
	// different SeqId
	BOOST_ASSERT( a._numbers.size() == b._numbers.size() );
	for( FileNumbers::Vec::const_iterator i = a._numbers.begin(), iEnd = a._numbers.end(), vi = b._numbers.begin(); i != iEnd; ++i, ++vi )
	{
		const std::size_t iPadding = extractPadding( i->second );
		const std::size_t viPadding = extractPadding( vi->second );
		if( iPadding < viPadding )
			return true;
		else if( iPadding > viPadding )
			return false;

		if( i->first < vi->first )
			return true;
		else if( i->first > vi->first )
			return false;
	}
	return false; // equals
}

bool FileNumbers::SortByDigit::operator()( const FileNumbers& a, const FileNumbers& b ) const
{
	// can't have multiple size, if multiple size they must have a
	// different SeqId
	BOOST_ASSERT( a._numbers.size() == b._numbers.size() );
	for( FileNumbers::Vec::const_iterator i = a._numbers.begin(), iEnd = a._numbers.end(), vi = b._numbers.begin(); i != iEnd; ++i, ++vi )
	{
		const std::size_t iMaxPadding = extractMaxPadding( i->second );
		const std::size_t viMaxPadding = extractMaxPadding( vi->second );
		if( iMaxPadding < viMaxPadding )
			return true;
		else if( iMaxPadding > viMaxPadding )
			return false;

		if( i->first < vi->first )
			return true;
		else if( i->first > vi->first )
			return false;
	}
	return false; // equals
}

std::ostream& operator<<(std::ostream& os, const FileNumbers& p)
{
    os << "[";
    BOOST_FOREACH( const FileNumbers::Vec::value_type & v, p._numbers )
		{
		    os << v.second << ",";
		}
    os << "]";
    return os;
}

}
}
