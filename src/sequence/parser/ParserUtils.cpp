/*
 * ParserUtils.cpp
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "ParserUtils.h"

#include <sequence/Sequence.h>

#include <boost/bind/mem_fn.hpp>
#include <boost/bind/bind.hpp>

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
        const Itr pastDigitEnd = find_if(current, end, isNonDigit);
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
        sets[i].insert(values[i]);
}

typedef vector<size_t> SizeVector;
typedef vector<bool> BoolVector;

static inline BoolVector generateKeepLookup(const SizeVector &v) {
    BoolVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i)
        result[i] = v[i] != 1;
    return result;
}

static inline SizeVector grabSetsSizes(const PatternAggregator::Sets& sets) {
    SizeVector sizes(sets.size());
    transform(sets.begin(), sets.end(), sizes.begin(), mem_fun_ref(&PatternAggregator::Set::size));
    return sizes;
}

static inline void instanciateNumberAt(string &key, const Location &location, unsigned int value) {
    details::CharStack<> stack(value);
    while (stack.size() < location.count)
        stack.push('0');
    char* ptr = &key[location.first];
    for (; !stack.empty(); stack.pop(), ++ptr)
        *ptr = stack.top();
}

PatternAggregator PatternAggregator::generateCopy(const BoolVector &keepLookup) const {
    string newKey = key;
    Locations newLocations;
    for (size_t locationIndex = 0; locationIndex < locationCount(); ++locationIndex) {
        const Location &location = locations[locationIndex];
        if (keepLookup[locationIndex])
            newLocations.push_back(location);
        else
            instanciateNumberAt(newKey, location, *(sets[locationIndex].begin()));
    }
    PatternAggregator result(newKey, newLocations);
    Values tmp;
    PatternAggregator::const_iterator itr = begin();
    for (size_t locationIndex = 0; itr != end(); ++itr, ++locationIndex, locationIndex %= locationCount()) {
        if (keepLookup[locationIndex])
            tmp.push_back(*itr);
        if (tmp.size() == result.locationCount()) {
            result.append(tmp);
            tmp.clear();
        }
    }
    return result;
}

PatternAggregator PatternAggregator::morphIfNeeded() const {
    const SizeVector setsSizes = grabSetsSizes(sets);
    const BoolVector keepLookup = generateKeepLookup(setsSizes);
    const size_t locationsToRemove = count(keepLookup.begin(), keepLookup.end(), false);
    assert(locationsToRemove>0);
    return generateCopy(keepLookup);
}

void FilenameAggregator::optimize() {
    vector<PatternAggregator> optimized;
    vector<string> toDelete;
    for (const_iterator itr = begin(); itr != end(); ++itr) {
        const PatternAggregator &current = itr->second;
        if (current.isValid())
            continue;
        const PatternAggregator newOne = current.morphIfNeeded();
        if (!newOne.isValid()) {
            printf("I'm only a poor little parser and I can't do anything with %s\n", current.key.c_str());
            continue;
        }
        optimized.push_back(newOne);
        toDelete.push_back(itr->first);
    }
    for (vector<string>::const_iterator itr = toDelete.begin(); itr != toDelete.end(); ++itr)
        erase(*itr);
    for (vector<PatternAggregator>::const_iterator itr = optimized.begin(); itr != optimized.end(); ++itr)
        insert(make_pair(itr->key, *itr));
}

static inline void convert(const FilenameAggregator::value_type & pair, std::vector<BrowseItem> &container, const boost::filesystem::path &path) {
    const PatternAggregator &pattern = pair.second;
    if (pattern.isSingleFile()) {
        container.push_back(sequence::create_file(path / pattern.key));
        return;
    }
}

void FilenameAggregator::appendResultsTo(std::vector<BrowseItem> &container, const boost::filesystem::path &path) const {
    for_each(begin(), end(), boost::bind(convert, _1, boost::ref(container), boost::ref(path)));
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

static inline void optimize(std::vector<BrowseItem>& results, FolderMap::value_type &pair) {
    FilenameAggregator &aggregator = pair.second;
    aggregator.optimize();
    aggregator.appendResultsTo(results, pair.first);
}

std::vector<BrowseItem> Parser::finalize() {
    std::vector<BrowseItem> results;
    for_each(folderMap.begin(), folderMap.end(), boost::bind(optimize, boost::ref(results), _1));
    return results;
}

} /* namespace parser */
} /* namespace sequence */
