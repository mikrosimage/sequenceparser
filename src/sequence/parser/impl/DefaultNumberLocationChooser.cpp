/*
 * DefaultNumberLocationChooser.cpp
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "DefaultNumberLocationChooser.h"

#include <sequence/parser/PatternAggregator.h>

#include <vector>
#include <set>
#include <iostream>

#include <cstdio>

using namespace std;

namespace sequence {
namespace parser {

const NumberLocation* chooseNumberLocation(const PatternKey& pattern) {
//    assert(!pattern.emptyLocation());
//
//    if (pattern.fileCount() <= 1)
//        return NULL; // only one file
//
//    const size_t locationCount = pattern.locationCount();
//    if (locationCount == 1)
//        return &pattern.numberLocations[0]; // only one location
//
//    typedef set<unsigned int> Set;
//    vector<Set> sets(locationCount);
//    typedef vector<unsigned int> V;
//    const V &values = pattern.values;
//    size_t selector = 0;
//    for (V::const_iterator itr = values.begin(); itr != values.end(); ++itr, ++selector, selector %= locationCount)
//        sets[selector].insert(*itr);
//
//    typedef vector<size_t> SetSizes;
//    SetSizes setSizes(sets.size());
//    transform(sets.begin(), sets.end(), setSizes.begin(), mem_fun_ref(&Set::size));
//    SetSizes::const_iterator pMax = max_element(setSizes.begin(), setSizes.end());
//    assert(pMax!=setSizes.end());
//    const size_t maxElementCount = count(setSizes.begin(), setSizes.end(), *pMax);
//
//    if (maxElementCount > 1) {
//        printf("%d locations in %s have exactly %d different values, can't decide which one is better : assuming single files\n", maxElementCount, pattern.key.c_str(), *pMax);
//        return NULL;
//    }
//
//    const size_t bestMatchIndex = std::distance(SetSizes::const_iterator(setSizes.begin()), pMax);
//    return &pattern.numberLocations[bestMatchIndex];
}

} /* namespace parser */
} /* namespace sequence */
