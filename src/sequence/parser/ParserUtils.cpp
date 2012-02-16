/*
 * ParserUtils.cpp
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "ParserUtils.h"

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

void extractPattern(const std::string &filename, std::string &key, NumberLocationInstances &values) {
    values.clear();
    key = filename;
    typedef string::iterator Itr;
    const Itr begin = key.begin();
    Itr current = begin;
    const Itr end = key.end();
    while ((current = find_if(current, end, isDigit)) != end) {
        NumberLocationInstance instance;
        instance.location.first = distance(begin, current);
        const Itr pastEnd = find_if(current, end, isNonDigit);
        instance.location.count = distance(current, pastEnd);
        instance.value = atoi(current, pastEnd);
        values.push_back(instance);
        fill(current, pastEnd, '#');
        current = pastEnd;
    }
}

PatternKey::PatternKey(const std::string& key, const NumberLocations &locations) :
                key(key), numberLocations(locations) {
}

void PatternAggregator::operator()(const boost::filesystem::path& path) {
//    boost::filesystem::path parent(path.parent_path());
//    FolderMap::iterator itr = folderMap.find(parent);
//    if (itr == folderMap.end())
//        itr = folderMap.insert(make_pair(parent, KeyMap())).first;
//    KeyMap &keyMap = itr->second;
//    PatternKey key(path.filename().string());
//    KeyMap::iterator keyItr = keyMap.find(key);
//    if (keyItr == keyMap.end())
//        keyItr = keyMap.insert(make_pair(key.key, key)).first;
//    else
//        keyItr->second.append(key);
}

} /* namespace parser */
} /* namespace sequence */
