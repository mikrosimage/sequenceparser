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

void extractPattern(std::string &pattern, Locations &locations, Values &values) {
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

static inline void write(unsigned int value, string &inString, const Location &atLocation) {
    sequence::details::CharStack<> stack(value);
    while (stack.size() < atLocation.count)
        stack.push('0');
    char* ptr = &inString[atLocation.first];
    for (; !stack.empty(); stack.pop(), ++ptr)
        *ptr = stack.top();
}

std::vector<Range> LocationValueSet::getRanges(LocationValueSet::value_type &step) const {
    assert(!isConstant());
    typedef std::vector<value_type> V;
    typedef V::const_iterator VItr;
    V derivative(size());
    adjacent_difference(begin(), end(), derivative.begin());
    step = *std::min_element(derivative.begin(), derivative.end());
    std::vector<Range> ranges;
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

static inline BoolVector generateKeepLookup(const SizeVector &v) {
    BoolVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i)
        result[i] = v[i] != 1;
    return result;
}

PatternAggregator PatternAggregator::createCopyWithLocations(const BoolVector &keepLocation) const {
    string newKey = key;
    Locations newLocations;
    for (size_t locationIndex = 0; locationIndex < locationCount(); ++locationIndex) {
        const Location &location = locations[locationIndex];
        if (keepLocation[locationIndex])
            newLocations.push_back(location);
        else {
            const value_type value = *(locationValueSets[locationIndex].begin());
            write(value, newKey, location);
        }
    }
    PatternAggregator result(newKey, newLocations);
    Values tmp;
    PatternAggregator::const_iterator itr = begin();
    for (size_t locationIndex = 0; itr != end(); ++itr, ++locationIndex, locationIndex %= locationCount()) {
        if (keepLocation[locationIndex])
            tmp.push_back(*itr);
        if (tmp.size() == result.locationCount()) {
            result.append(tmp);
            tmp.clear();
        }
    }
    return result;
}

PatternAggregator PatternAggregator::removeConstantLocations() const {
    const SizeVector setsSizes = grabSetsSizes(locationValueSets);
    const BoolVector keepLocations = generateKeepLookup(setsSizes);
    return createCopyWithLocations(keepLocations);
}

void SequenceDetector::reduce() {
    vector<PatternAggregator> toAdd;
    vector<string> toDelete;
    for (const_iterator itr = begin(); itr != end(); ++itr) {
        const PatternAggregator &current = itr->second;
        if (current.isValid())
            continue;
        const PatternAggregator newOne = current.removeConstantLocations();
        if (!newOne.isValid()) {
            printf("I'm only a poor little parser and I can't do a thing with %s\n", current.key.c_str());
            continue;
        }
        toAdd.push_back(newOne);
        toDelete.push_back(itr->first);
    }
    for (vector<string>::const_iterator itr = toDelete.begin(); itr != toDelete.end(); ++itr)
        erase(*itr);
    for (vector<PatternAggregator>::const_iterator itr = toAdd.begin(); itr != toAdd.end(); ++itr)
        insert(make_pair(itr->key, *itr));
}

void SequenceDetector::process(const value_type& pair) {
    const PatternAggregator &pattern = pair.second;
    assert(pattern.isValid());
    if (pattern.isSingleFile()) {
        results.push_back(sequence::create_file(path / pattern.key));
        return;
    }
    const SequencePattern sequencePattern = sequence::parsePattern(pattern.key);
    unsigned int step = 0;
    const std::vector<Range> ranges = pattern.getSelectedSet().getRanges(step);
    for (std::vector<Range>::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr)
        results.push_back(create_sequence(path, sequencePattern, *itr, step));
}

const std::vector<BrowseItem>& SequenceDetector::getResults() {
    if (results.empty()) {
        reduce();
        for_each(begin(), end(), boost::bind(&SequenceDetector::process, this, _1));
    }
    return results;
}

const PatternAggregator& SequenceDetector::operator()(std::string key) {
    if (!results.empty())
        throw std::logic_error("Already processed");
    assert(key.find('/')==string::npos);
    assert(key.find('\\')==string::npos);
    extractPattern(key, extractedLocations, extractedValues);
    SequenceDetector::iterator keyItr = find(key);
    if (keyItr == end())
        keyItr = insert(make_pair(key, PatternAggregator(key, extractedLocations))).first;
    keyItr->second.append(extractedValues);
    return keyItr->second;
}

} /* namespace details */
} /* namespace parser */
} /* namespace sequence */
