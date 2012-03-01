/*
 * Browser.cpp
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "Browser.h"
#include "ParserUtils.h"

#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

#include <string>
#include <stdexcept>

using namespace std;
using namespace sequence;
using namespace sequence::parser::details;
using namespace boost::filesystem;

namespace sequence {
namespace parser {

static inline path getDirectory(const char* directory) {
    const path folder(directory);
    if (!exists(folder))
        throw std::ios_base::failure(string("Unable to browse ") + folder.string());
    return folder;
}

static inline void changeTypeIfNeeded(BrowseItem &item) {
    if (item.type == UNITFILE && is_directory(item.path))
        item.type = FOLDER;
}

std::vector<BrowseItem> browse(const char* directory, bool recursive) {
    const path folder = getDirectory(directory);
    Parser parser;
    if (recursive)
        for_each(recursive_directory_iterator(folder), //
                 recursive_directory_iterator(), //
                 parser.functor());
    else
        for_each(directory_iterator(folder), //
                 directory_iterator(), //
                 parser.functor());
    vector<BrowseItem> items = parser.getResults();
    for_each(items.begin(), items.end(), &changeTypeIfNeeded);
    return items;
}

} /* namespace parser */
} /* namespace sequence */
