#include "Sequence.h"
#include "DisplayUtils.h"

#include <algorithm>
#include <sstream>

using namespace std;

namespace sequence {

SequencePattern parsePattern(const std::string& filename) {
    typedef string::const_iterator CItr;
    const CItr paddingBegin = std::find(filename.begin(), filename.end(), gPaddingChar);
    if (paddingBegin == filename.end())
        throw runtime_error(string("Unable to find '#' in filename '") + filename + '\'');
    const CItr paddingEnd = std::upper_bound(paddingBegin, filename.end(), gPaddingChar);
    return SequencePattern(string(filename.begin(), paddingBegin), //
                           string(paddingEnd, filename.end()), //
                           distance(paddingBegin, paddingEnd));
}

std::string instanciatePattern(const SequencePattern &pattern, unsigned int frame) {
    string result;
    result.reserve(100);
    result += pattern.prefix;
    const bool padding = pattern.padding > 1;
    details::CharStack<> stack(frame);
    if (padding) {
        if (stack.size() > pattern.padding) {
            ostringstream ss;
            ss << "Unable to put " << frame << " in pattern '" << pattern << '\"';
            throw runtime_error(ss.str());
        }
        while (stack.size() < pattern.padding)
            stack.push('0');
    }
    for (; !stack.empty(); stack.pop())
        result.push_back(stack.top());
    result += pattern.suffix;
    return result;
}

} // namespace sequence
