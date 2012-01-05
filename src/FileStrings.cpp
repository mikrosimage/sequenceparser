#include <FileStrings.hpp>

namespace sequenceParser {

std::ostream& operator<<( std::ostream& os, const FileStrings& p )
{
    os << "[";
    std::for_each(p._id.begin(), p._id.end(), os << boost::lambda::_1 << ",");
    os << "]";
    return os;
}

}

