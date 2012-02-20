/*
 * Browser.cpp
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "Browser.h"
#include "ParserUtils.h"

#include <string>
#include <stdexcept>
#include <boost/filesystem.hpp>

using namespace std;
using namespace sequence;
using namespace sequence::parser::details;
using namespace boost::filesystem;

namespace sequence {
namespace parser {

static inline path getDirectory(const char* directory){
    const path folder(directory);
    if (!exists(folder))
           throw std::ios_base::failure(string("Unable to browse ") + folder.string());
    return folder;
}

std::vector<BrowseItem> browse(const char* directory) {
    const path folder = getDirectory(directory);
    SequenceDetector detector(folder);
    directory_iterator end;
    for (directory_iterator itr(folder); itr != end; ++itr)
        detector(itr->path().filename().string());
    return detector.getResults();
}

std::vector<BrowseItem> browseRecursive(const char* directory) {
    const path folder = getDirectory(directory);
    SequenceDetector detector(folder);
    directory_iterator end;
    path lastParent = folder.parent_path();
    for (directory_iterator itr(folder); itr != end; ++itr)
        detector(itr->path().filename().string());
    return detector.getResults();
}

} /* namespace parser */
} /* namespace sequence */
