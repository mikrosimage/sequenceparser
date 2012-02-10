/*
 * Item.h
 *
 *  Created on: Feb 9, 2012
 *      Author: clitte
 */

#ifndef ITEM_H_
#define ITEM_H_

#include <boost/filesystem/path.hpp>
#include <string>

namespace sequence {

enum ItemType { UNDEFINED, FOLDER, SEQUENCE, UNITFILE};

struct Item {
	ItemType type;
	boost::filesystem::path path;
	struct Sequence {
		unsigned int first;
		unsigned int last;
		unsigned short step;
		unsigned char padding;
		std::string prefix;
		std::string suffix;
		Sequence() : first(0), last(0), step(0), padding(0) {}
	} sequence;
	Item() : type(UNDEFINED) {}
};

Item folder(const boost::filesystem::path &path);
Item file(const boost::filesystem::path &filename);
Item sequence(const boost::filesystem::path &path, //
		unsigned int first, //
		unsigned int last, //
		unsigned char padding, //
		const std::string &prefix, //
		const std::string &suffix, //
		const unsigned short step = 1 //
		);

} /* namespace sequence */
#endif /* ITEM_H_ */
