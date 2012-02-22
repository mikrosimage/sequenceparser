/*
 * ParserUtils.cpp
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "ParserUtils.h"

#include <sequence/Sequence.h>
#include <sequence/DisplayUtils.h>

#include <boost/bind/mem_fn.hpp>
#include <boost/bind/bind.hpp>

#include <algorithm>
#include <numeric>
#include <cctype>
#include <cassert>
#include <cstdio>

using namespace std;

namespace sequence {
namespace parser {
namespace details {

static inline bool isDigit(string::value_type c) {
    return c >= '0' && c <= '9';
}

static inline size_t atoi(string::const_iterator begin, const string::const_iterator end) {
    size_t value = 0;
    for (; begin != end; ++begin)
        value = value * 10 + (*begin - '0');
    return value;
}

void extractPattern(string &pattern, Locations &locations, Values &values) {
    locations.clear();
    values.clear();
    typedef string::iterator Itr;
    const Itr begin = pattern.begin();
    Itr current = begin;
    const Itr end = pattern.end();
    while ((current = find_if(current, end, isDigit)) != end) {
        Location location;
        location.first = distance(begin, current);
        const Itr pastDigitEnd = find_if(current, end, not1(ptr_fun(isDigit)));
        location.count = distance(current, pastDigitEnd);
        values.push_back(atoi(current, pastDigitEnd));
        locations.push_back(location);
        fill(current, pastDigitEnd, '#');
        current = pastDigitEnd;
    }
}

void PatternAggregator::append(const Values& values) {
    assert(values.size()==locationCount());
    insert(end(), values.begin(), values.end());
    for (size_t i = 0; i < values.size(); ++i)
        locationValueSets[i].insert(values[i]);
}

typedef vector<size_t> SizeVector;

static inline SizeVector grabSetsSizes(const PatternAggregator::LocationValueSets& sets) {
    SizeVector sizes(sets.size());
    transform(sets.begin(), sets.end(), sizes.begin(), mem_fun_ref(&LocationValueSet::size));
    return sizes;
}

static inline void overwrite(unsigned int value, string &inString, const Location &atLocation) {
    sequence::details::CharStack<> stack(value);
    while (stack.size() < atLocation.count)
        stack.push('0');
    char* ptr = &inString[atLocation.first];
    for (; !stack.empty(); stack.pop(), ++ptr)
        *ptr = stack.top();
}

vector<Range> LocationValueSet::getConsecutiveRanges(LocationValueSet::value_type &step) const {
    assert(!isConstant());
    typedef vector<value_type> V;
    typedef V::const_iterator VItr;
    V derivative(size());
    adjacent_difference(begin(), end(), derivative.begin());
    step = max(value_type(1), *min_element(derivative.begin(), derivative.end()));
    vector<Range> ranges;
    const_iterator itr = begin();
    VItr d_itr = derivative.begin();
    for (; d_itr != derivative.end(); ++d_itr, ++itr) {
        if (*d_itr == step && !ranges.empty())
            ranges.back().last = *itr;
        else
            ranges.push_back(Range(*itr, *itr));
    }
    return ranges;
}

typedef vector<bool> BoolVector;

static inline BoolVector sizeEqualsToOne(const SizeVector &v) {
    BoolVector result(v.size());
    typedef equal_to<unsigned int> Equal;
    transform(v.begin(), v.end(), result.begin(), binder1st<Equal>(Equal(), 1));
    return result;
}

PatternAggregator PatternAggregator::discard(const BoolVector &discardLocation) const {
    string newKey = key;
    Locations newLocations;
    for (size_t locationIndex = 0; locationIndex < locationCount(); ++locationIndex) {
        const Location &location = locations[locationIndex];
        if (discardLocation[locationIndex]) {
            const value_type value = *(locationValueSets[locationIndex].begin());
            overwrite(value, newKey, location);
        } else {
            newLocations.push_back(location);
        }
    }
    PatternAggregator result(newKey, newLocations);
    Values tmp;
    PatternAggregator::const_iterator itr = begin();
    for (size_t locationIndex = 0; itr != end(); ++itr, ++locationIndex, locationIndex %= locationCount()) {
        if (!discardLocation[locationIndex])
            tmp.push_back(*itr);
        if (tmp.size() == result.locationCount()) {
            result.append(tmp);
            tmp.clear();
        }
    }
    return result;
}

PatternAggregator PatternAggregator::removeConstantLocations() const {
    const BoolVector discardLocations = isSingleFile() ? //
                    BoolVector(locationCount(), true) : // single file, discarding all values
                    sizeEqualsToOne(grabSetsSizes(locationValueSets)); // keeping sets with more than one value
    return discard(discardLocations);
}

void SequenceDetector::reduce() {
    vector<PatternAggregator> toAdd;
    vector<string> toDelete;
    for (const_iterator itr = begin(); itr != end(); ++itr) {
        const PatternAggregator &current = itr->second;
        const PatternAggregator newOne = current.removeConstantLocations();
        if (newOne.isReady()) {
            toAdd.push_back(newOne);
        } else { // still more than one location, splitting according to first locations
            printf("I'm only a poor little parser and I can't do a thing with %s\n", current.key.c_str());
            continue;
        }
        toDelete.push_back(itr->first);
    }
    for (vector<string>::const_iterator itr = toDelete.begin(); itr != toDelete.end(); ++itr)
        erase(*itr);
    for (vector<PatternAggregator>::const_iterator itr = toAdd.begin(); itr != toAdd.end(); ++itr)
        insert(make_pair(itr->key, *itr));
}

void SequenceDetector::process(const value_type& pair) {
    const PatternAggregator &pattern = pair.second;
    if (!pattern.isReady()) {
        printf("Skipping pattern %s\n", pattern.key.c_str());
        return;
    }
    if (pattern.isSingleFile()) {
        results.push_back(sequence::create_file(path / pattern.key));
        return;
    }
    const SequencePattern sequencePattern = sequence::parsePattern(pattern.key);
    unsigned int step = 0;
    const vector<Range> ranges = pattern.getSelectedSet().getConsecutiveRanges(step);
    for (vector<Range>::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr)
        results.push_back(create_sequence(path, sequencePattern, *itr, step));
}

const vector<BrowseItem>& SequenceDetector::getResults() {
    if (results.empty()) {
        reduce();
        for_each(begin(), end(), boost::bind(&SequenceDetector::process, this, _1));
    }
    return results;
}

const PatternAggregator& SequenceDetector::operator()(string key) {
    if (!results.empty())
        throw logic_error("Already processed");
    assert(key.find('/')==string::npos);
    assert(key.find('\\')==string::npos);
    extractPattern(key, extractedLocations, extractedValues);
    SequenceDetector::iterator keyItr = find(key);
    if (keyItr == end())
        keyItr = insert(make_pair(key, PatternAggregator(key, extractedLocations))).first;
    keyItr->second.append(extractedValues);
    return keyItr->second;
}

using namespace boost::filesystem;

void Parser::operator ()(const path &_path) {
    const path parent = _path.parent_path();
    iterator found = find(parent);
    if (found == end())
        found = insert(make_pair(parent, SequenceDetector(parent))).first;
    found->second(_path.filename().string());
}

std::vector<BrowseItem> Parser::getResults() {
    std::vector<BrowseItem> items;
    items.reserve(200);
    for (iterator itr = begin(); itr != end(); ++itr) {
        const vector<BrowseItem> &currentItems = itr->second.getResults();
        items.insert(items.end(), currentItems.begin(), currentItems.end());
    }
    return items;
}

} /* namespace details */
} /* namespace parser */
} /* namespace sequence */
