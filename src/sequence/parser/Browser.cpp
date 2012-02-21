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
#include <iostream>
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

std::vector<BrowseItem> browse(const char* directory, bool recursive) {
    const path folder = getDirectory(directory);
    recursive_directory_iterator end;
    path lastParent = folder.parent_path();
    typedef boost::unordered_map<path, SequenceDetector> Map;
    typedef Map::iterator MapItr;
    Map detectors;
    for (recursive_directory_iterator itr(folder, recursive ? symlink_option::recurse : symlink_option::no_recurse); itr != end; ++itr) {
        const path& _path = itr->path();
        const path parent = _path.parent_path();
        MapItr found = detectors.find(parent);
        if (found == detectors.end())
            found = detectors.insert(make_pair(parent, SequenceDetector(parent))).first;
        found->second(_path.filename().string());
    }
    vector<BrowseItem> items;
    items.reserve(200);
    for (MapItr itr = detectors.begin(); itr != detectors.end(); ++itr) {
        const vector<BrowseItem> &currentItems = itr->second.getResults();
        items.insert(items.end(), currentItems.begin(), currentItems.end());
    }
    for (vector<BrowseItem>::iterator itr = items.begin(); itr != items.end(); ++itr)
        if (itr->type == UNITFILE && is_directory(itr->path))
            itr->type = FOLDER;
    return items;
}

} /* namespace parser */
} /* namespace sequence */
