#include "DisplayUtils.h"

#include "Range.h"
#include "Sequence.h"

#include <boost/filesystem.hpp>
#include <ostream>

using namespace std;

namespace sequence {

ostream& operator<<(ostream &stream, const Range &range) {
    stream << '[' << range.first << ':' << range.last << ']';
    return stream;
}

const char* toString(const BrowseItemType type) {
    switch (type) {
        case FOLDER:
            return "FOLDER";
        case UNITFILE:
            return "UNITFILE";
        case SEQUENCE:
            return "SEQUENCE";
        case UNDEFINED:
            return "UNDEFINED";
        default:
            return "!BAD_TYPE!";
    }
}

ostream& operator<<(ostream &stream, const BrowseItemType &type) {
    stream << toString(type);
    return stream;
}

ostream& operator<<(ostream &stream, const SequencePattern &pattern) {
    stream << pattern.prefix;
    const size_t padding = pattern.padding == 0 ? 1 : pattern.padding;
    for (size_t i = 0; i < padding; ++i)
        stream << gPaddingChar;
    stream << pattern.suffix;
    return stream;
}

ostream& operator<<(ostream &stream, const Sequence &sequence) {
    stream << sequence.pattern << ' ' << sequence.range;
    if (sequence.step > 1)
        stream << " (" << sequence.step << ')';
    return stream;
}

ostream& operator<<(ostream &stream, const BrowseItem &item) {
    stream << item.type << ' ' << item.path.string();

    if (item.type == SEQUENCE)
        stream << boost::filesystem::path("/").make_preferred().string() << item.sequence;

    return stream;
}

} /* namespace sequence */
