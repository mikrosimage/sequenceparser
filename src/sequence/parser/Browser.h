/*
 * Browser.h
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef BROWSER_H_
#define BROWSER_H_

#include <sequence/BrowseItem.h>
#include <vector>

namespace sequence {

typedef std::vector<sequence::BrowseItem> BrowseItems;

namespace parser {

BrowseItems browse(const char* directory, bool recursive = false);

} /* namespace parser */

/**
 * removes the items who satisfies Func from le items vector.
 * Func is a unary_function taking a const sequence::BrowseItem &
 */
template<typename Func>
static inline void filterOut(BrowseItems &items, Func f) {
    items.erase(remove_if(items.begin(), items.end(), f), items.end());
}

} /* namespace sequence */
#endif /* BROWSER_H_ */
