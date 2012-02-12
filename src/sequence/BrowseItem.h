#ifndef BROWSEITEM_H_
#define BROWSEITEM_H_

#include "Sequence.h"

#include <boost/filesystem.hpp>
#include <string>

namespace sequence {

enum BrowseItemType {
    UNDEFINED, FOLDER, SEQUENCE, UNITFILE
};

struct BrowseItem {
    BrowseItemType type;
    boost::filesystem::path path;
    Sequence sequence;
    BrowseItem() : type(UNDEFINED) {}
    BrowseItem(const BrowseItemType type, const boost::filesystem::path &path, const Sequence &sequence = Sequence()) : type(type), path(path), sequence(sequence) {}
};

BrowseItem folder(const boost::filesystem::path &path);
BrowseItem file(const boost::filesystem::path &filename);
BrowseItem sequence(const boost::filesystem::path &path, //
                const SequencePattern &pattern, //
                const Range &range, //
                const unsigned short step = 1);

} /* namespace sequence */

#endif /* BROWSEITEM_H_ */
