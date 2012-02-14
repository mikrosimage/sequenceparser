#ifndef BROWSEITEM_H_
#define BROWSEITEM_H_

#include "Sequence.h"

#include <boost/filesystem/path.hpp>

namespace sequence {

/**
 * Identifies the type of a BrowseItem
 */
enum BrowseItemType {
    UNDEFINED, FOLDER, SEQUENCE, UNITFILE
};

/**
 * A lightweight structure representing an Item found by the sequence parser.
 * The embedded Sequence object is used if BrowseItem::type is SEQUENCE.
 *
 * No need for inheritance here. The object is lightweight and straightforward.
 */
struct BrowseItem {
    BrowseItemType type;
    boost::filesystem::path path;
    Sequence sequence;
    BrowseItem() : type(UNDEFINED) {}
    BrowseItem(const BrowseItemType type, const boost::filesystem::path &path, const Sequence &sequence = Sequence()) : type(type), path(path), sequence(sequence) {}
};

/**
 * Helper to quickly create a folder
 */
BrowseItem create_folder(const boost::filesystem::path &path);

/**
 * Helper to quickly create a unique file
 */
BrowseItem create_file(const boost::filesystem::path &filename);

/**
 * Helper to quickly create a sequence
 */
BrowseItem create_sequence(const boost::filesystem::path &path, //
                const SequencePattern &pattern, //
                const Range &range, //
                const unsigned short step = 1);

} /* namespace sequence */

#endif /* BROWSEITEM_H_ */
