#ifndef RANGE_H_
#define RANGE_H_

#include <stdexcept>

namespace sequence {

struct Range {
    unsigned int first;
    unsigned int last;
    Range() : first(0), last(0) { }
    Range(unsigned int first, unsigned int last) : first(first), last(last) {
        if(!valid()) throw std::logic_error("Invalid range, 'first' must not be greater than 'last'");
    }
    static Range weak(unsigned int first, unsigned int count);
    bool contains(unsigned int frame) const { return first <= frame && frame <= last; }
    bool valid() const { return first<=last; }
    unsigned int duration() const;
    unsigned int offsetClampFrame(unsigned int current, int offset) const;
    unsigned int offsetLoopFrame(unsigned int current, int offset) const;
    unsigned int clampFrame(unsigned int current) const;
};

unsigned int interpolateSource(const unsigned int atRecIndex, const Range &source, const Range &record, const bool reverse);

} /* namespace sequence */
#endif /* RANGE_H_ */
