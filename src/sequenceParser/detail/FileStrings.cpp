#include "FileStrings.hpp"

#include <boost/functional/hash.hpp>

namespace sequenceParser {
namespace detail {

std::size_t FileStrings::getHash() const
{
	std::size_t seed = 0;

	BOOST_FOREACH( const Vec::value_type & i, _id )
	{
		boost::hash_combine( seed, i );
		boost::hash_combine( seed, 1 ); // not like the hash of the concatenation of _id
	}
	return seed;
}

std::ostream& operator<<( std::ostream& os, const FileStrings& p )
{
	os << "[";
	std::for_each( p._id.begin(), p._id.end(), os << boost::lambda::_1 << "," );
	os << "]";
	return os;
}

}
}
