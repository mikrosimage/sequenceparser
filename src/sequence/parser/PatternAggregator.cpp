/*
 * PatternAggregator.cpp
 *
 *  Created on: Feb 16, 2012
 *      Author: Guillaume Chatelet
 */

#include "PatternAggregator.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cassert>

using namespace std;

namespace sequence {
namespace parser {

Locations::Locations() :
                count(0) {
}

void Locations::push_back(const Location& location) {
    assert(count<MAX_LOCATIONS);
    locations[count++] = location;
}

size_t Locations::size() const {
    return count;
}

const Location& Locations::operator[](size_t index) const {
    assert(index<count);
    return locations[index];
}

PatternAggregator::PatternAggregator() {
}

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

PatternKey::PatternKey(const string& filename) :
                key(filename) {
    typedef string::iterator Itr;
    const Itr begin = key.begin();
    Itr current = begin;
    const Itr end = key.end();
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

void PatternAggregator::operator()(const boost::filesystem::path& path) {
    boost::filesystem::path parent(path.parent_path());
    FolderMap::iterator itr = folderMap.find(parent);
    if (itr == folderMap.end())
        itr = folderMap.insert(make_pair(parent, KeyMap())).first;
    KeyMap &keyMap = itr->second;
    PatternKey key(path.filename().string());
    KeyMap::iterator keyItr = keyMap.find(key.key);
    if (keyItr == keyMap.end())
        keyItr = keyMap.insert(make_pair(key.key, key)).first;
    else {
        vector<unsigned int> &values(keyItr->second.values);
        values.insert(values.end(), key.values.begin(), key.values.end());
    }
}

} /* namespace parser */
} /* namespace sequence */
