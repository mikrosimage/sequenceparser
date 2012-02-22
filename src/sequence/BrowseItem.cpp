#include "BrowseItem.h"

using namespace std;
using boost::filesystem::path;

namespace sequence {

BrowseItem create_folder(const path &path) {
    return BrowseItem(FOLDER, path);
}

BrowseItem create_file(const path &filename) {
    return BrowseItem(UNITFILE, filename);
}

BrowseItem create_sequence(const path &path, //
                const SequencePattern &pattern, //
                const Range &range, //
                const unsigned short step) {
    return BrowseItem(SEQUENCE, path, Sequence(pattern, range, step));
}

string BrowseItem::extension() const{
    switch (type) {
        case sequence::UNITFILE:
            return path.extension().string();
        case sequence::SEQUENCE:
            return boost::filesystem::path(sequence.pattern.suffix).extension().string();
        case sequence::FOLDER:
        case sequence::UNDEFINED:
        default:
            return "";
    }
}

} /* namespace sequence */
