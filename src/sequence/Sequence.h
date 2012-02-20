#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Range.h"

#include <boost/filesystem/path.hpp>

#include <string>
#include <utility>
#include <cassert>

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

namespace details {

template<size_t count=32>
struct CharStack {
    CharStack(unsigned int value) : index(0) {
        for (; value; value /= 10)
            push("0123456789"[value % 10]);
    }
    void push(char c) { assert(index<count); buffer[index++] = c; }
    size_t size() const { return index; }
    bool empty() const { return index == 0; }
    const char top() const { return buffer[index - 1]; }
    void pop() { assert(index!=0); --index; }
private:
    char buffer[count];
    size_t index;
};

}  // namespace details

} // namespace sequence

#endif /* SEQUENCE_H_ */
