/*
 * Item.cpp
 *
 *  Created on: Feb 9, 2012
 *      Author: clitte
 */

#include "Item.h"

using namespace std;
using boost::filesystem::path;


namespace sequence {

Item folder(const path &path) {
	Item item;
	item.type = FOLDER;
	item.path = path;
	return item;
}

Item file(const path &filename) {
	Item item;
	item.type = UNITFILE;
	item.path = filename;
	return item;
}

Item sequence(const path &path, //
		unsigned int first, //
		unsigned int last, //
		unsigned char padding, //
		const string &prefix, //
		const string &suffix, //
		const unsigned short step //
		) {
	assert(first<=last);
	Item item;
	item.type = SEQUENCE;
	item.path = path;
	item.sequence.first = first;
	item.sequence.last = last;
	item.sequence.step = step;
	item.sequence.padding = padding;
	item.sequence.prefix = prefix;
	item.sequence.suffix = suffix;
	return item;
}

} /* namespace sequence */
