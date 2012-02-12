#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Range.h"

#include <boost/filesystem.hpp>

#include <string>

namespace sequence {

const char gPaddingChar = '#';

struct SequencePattern {
    std::string prefix;
    std::string suffix;
    unsigned char padding;
    SequencePattern() : padding(1) {}
    SequencePattern(std::string prefix, std::string suffix, unsigned char padding = 1) : prefix(prefix), suffix(suffix), padding(padding) {}
};

struct Sequence {
    SequencePattern pattern;
    Range range;
    unsigned short step;
    Sequence() : step(1){}
    Sequence(const SequencePattern &pattern, const Range &range = Range(), unsigned short step=1) : pattern(pattern), range(range), step(step){}
};

std::pair<boost::filesystem::path, SequencePattern> parsePattern(const std::string& absoluteFilename);

std::string instanciatePattern(const SequencePattern &pattern, unsigned int frame);

} // namespace sequence

#endif /* SEQUENCE_H_ */
