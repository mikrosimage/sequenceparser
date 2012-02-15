#include "Range.h"
#include <boost/rational.hpp>
#include <cassert>
#include <stdexcept>
#include <climits>

using namespace std;

namespace sequence {

unsigned int interpolateSource(const unsigned int recFrame, const Range &source, const Range &record, const bool reverse) {
    assert(source.valid());
    assert(record.valid());
    assert(record.contains(recFrame));
    typedef boost::rational<int64_t> Rational;
    Rational recDuration = record.last - record.first;
    Rational srcDuration = source.last - source.first;
    if(recDuration>srcDuration){
    	++recDuration;
    	++srcDuration;
    }
    const Rational recOffset = reverse ? record.last - recFrame : recFrame - record.first;
    const Rational srcOffset = recDuration == 0 ? 0 : (recOffset * srcDuration / recDuration);
    const Rational srcFrame = srcOffset + source.first;
    return boost::rational_cast<unsigned int>(srcFrame);
}

unsigned int Range::duration() const {
    assert(valid());
    assert(last-first!=unsigned(-1));
    return last - first + 1;
}

static inline unsigned int loopReducedOffset(const Range &range, int32_t offset) {
    assert(range.valid());
    const bool forward = offset > 0;
    const unsigned int positiveOffset = forward ? offset : -offset;
    return positiveOffset % range.duration();
}

template<bool isCycling>
static inline unsigned int offsetFrame(const Range &range, unsigned int current, int offset) {
    assert(range.valid());
    assert(range.contains(current));
    if (offset == 0)
        return current;
    // offset frame is within [first,last]
    const bool forward = offset > 0;
    const uint32_t positiveOffset = forward ? offset : -offset;
    const uint32_t distanceToLast = Range(current, range.last).duration();
    const uint32_t distanceToFirst = Range(range.first,current).duration();
    const uint32_t semiRangeDuration = forward ? distanceToLast : distanceToFirst;
    // inside
    if (positiveOffset < semiRangeDuration)
        return forward ? current + positiveOffset : current - positiveOffset;
    const unsigned int reduced = loopReducedOffset(range, offset);
    if (forward)
        return isCycling ? range.first + reduced - distanceToLast : range.last;
    else
        return isCycling ? range.last - reduced + distanceToFirst : range.first;
}

unsigned int Range::offsetClampFrame(unsigned int current, int offset) const {
    return offsetFrame<false>(*this, current, offset);
}

unsigned int Range::offsetLoopFrame(unsigned int current, int offset) const {
    return offsetFrame<true>(*this, current, offset);
}

} /* namespace sequence */
