#ifndef DISPLAYUTILS_H_
#define DISPLAYUTILS_H_

#include "Range.h"
#include "Sequence.h"
#include "BrowseItem.h"
#include <ostream>

namespace sequence {

const char* toString(const BrowseItemType type);

std::ostream& operator<<(std::ostream &stream, const Range &range);
std::ostream& operator<<(std::ostream &stream, const SequencePattern &pattern);
std::ostream& operator<<(std::ostream &stream, const Sequence &sequence);
std::ostream& operator<<(std::ostream &stream, const BrowseItemType &type);
std::ostream& operator<<(std::ostream &stream, const BrowseItem &item);

} /* namespace sequence */
#endif /* DISPLAYUTILS_H_ */
