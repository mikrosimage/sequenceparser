#include "DisplayUtils.h"

namespace sequence {

std::ostream& operator<<(std::ostream &stream, const Range &range) {
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

std::ostream& operator<<(std::ostream &stream, const BrowseItemType &type) {
    stream << toString(type);
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const SequencePattern &pattern) {
    stream << pattern.prefix;
    const size_t padding = pattern.padding == 0 ? 1 : pattern.padding;
    for (size_t i = 0; i < padding; ++i)
        stream << gPaddingChar;
    stream << pattern.suffix;
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const Sequence &sequence) {
    stream << sequence.pattern << ' ' << sequence.range;
    if (sequence.step > 1)
        stream << " (" << sequence.step << ')';
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const BrowseItem &item) {
    stream << item.type << ' ' << item.path;

    if (item.type == SEQUENCE)
        stream << ' ' << item.sequence;

    return stream;
}

} /* namespace sequence */
