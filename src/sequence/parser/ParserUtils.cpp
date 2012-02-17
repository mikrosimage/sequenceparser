/*
 * ParserUtils.cpp
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "ParserUtils.h"

#include <sequence/Sequence.h>

#include <boost/bind/mem_fn.hpp>

#include <algorithm>
#include <cctype>
#include <cassert>
#include <cstdio>

using namespace std;

namespace sequence {
namespace parser {

static inline bool isDigit(string::value_type c) {
    return c >= '0' && c <= '9';
}

static inline bool isNonDigit(string::value_type c) {
    return !isDigit(c);
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
        const Itr pastEnd = find_if(current, end, isNonDigit);
        location.count = distance(current, pastEnd);
        values.push_back(atoi(current, pastEnd));
        locations.push_back(location);
        fill(current, pastEnd, '#');
        current = pastEnd;
    }
}

void PatternAggregator::append(const Values& values) {
    assert(values.size()==locationCount());
    insert(end(), values.begin(), values.end());
    for (size_t i = 0; i < values.size(); ++i)
        sets[i].insert(values[i]);
}

typedef vector<size_t> SizeVector;
typedef set<size_t> SizeSet;

static inline SizeSet keepIndices(const SizeVector &v) {
    SizeSet result;
    const SizeVector::const_iterator begin = v.begin();
    SizeVector::const_iterator current = begin;
    const SizeVector::const_iterator end = v.end();
    for (; current != end; ++current)
        if (*current != 1)
            result.insert(distance(begin, current));
    return result;
}

static inline void instanciateNumberAt(string &key, const Location &location, unsigned int value) {
    details::CharStack<char, 32> stack(value);
    while (stack.size() < location.count)
        stack.push('0');
    char* ptr = &key[location.first];
    for (; !stack.empty(); stack.pop(), ++ptr)
        *ptr = stack.top();
}

PatternAggregator PatternAggregator::optimize() const {
    SizeVector setSizes(sets.size());
    transform(sets.begin(), sets.end(), setSizes.begin(), mem_fun_ref(&Set::size));
    const SizeSet indices(keepIndices(setSizes));
    string newKey = key;
    Locations newLocations;
    for (size_t i = 0; i < locationCount(); ++i) {
        const bool suppressLocation = indices.count(i) == 0;
        const Location &location = locations[i];
        if (suppressLocation)
            instanciateNumberAt(newKey, location, *sets[i].begin());
        else
            newLocations.push_back(location);
    }
    PatternAggregator result(newKey, newLocations);
    PatternAggregator::const_iterator itr = begin();
    size_t selector = 0;
    Values tmp;
    for (; itr != end(); ++itr, ++selector, selector %= locationCount()) {
        if (indices.count(selector) > 0)
            tmp.push_back(*itr);
        if (tmp.size() == result.locationCount()) {
            result.append(tmp);
            tmp.clear();
        }
    }
    return result;
}

void FilenameAggregator::optimize() {
    vector<PatternAggregator> optimized;
    vector<string> toDelete;
    for (const_iterator itr = begin(); itr != end(); ++itr) {
        const PatternAggregator &current = itr->second;
        if (current.isOptimized())
            continue;
        optimized.push_back(current.optimize());
        toDelete.push_back(itr->first);
    }
    for (vector<string>::const_iterator itr = toDelete.begin(); itr != toDelete.end(); ++itr)
        erase(*itr);
    for (vector<PatternAggregator>::const_iterator itr = optimized.begin(); itr != optimized.end(); ++itr)
        insert(make_pair(itr->key, *itr));
}

const PatternAggregator& FilenameAggregator::operator()(std::string key) {
    extractPattern(key, tmpLocations, tmpValues);
    FilenameAggregator::iterator keyItr = find(key);
    if (keyItr == end())
        keyItr = insert(make_pair(key, PatternAggregator(key, tmpLocations))).first;
    keyItr->second.append(tmpValues);
    return keyItr->second;
}

const PatternAggregator& Parser::operator()(const boost::filesystem::path& path) {
    return folderMap[path.parent_path()](path.filename().string());
}

static inline void optimize(FolderMap::value_type &pair) {
    pair.second.optimize();
}

std::vector<BrowseItem> Parser::finalize() {
    for_each(folderMap.begin(), folderMap.end(), &optimize);
    return std::vector<BrowseItem>();
}

} /* namespace parser */
} /* namespace sequence */
