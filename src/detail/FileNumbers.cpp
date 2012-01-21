#include "FileNumbers.hpp"

namespace sequenceParser {
namespace detail {


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
