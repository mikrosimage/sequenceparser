/*
 * DefaultNumberLocationChooser.h
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef DEFAULTNUMBERLOCATIONCHOOSER_H_
#define DEFAULTNUMBERLOCATIONCHOOSER_H_

namespace sequence {
namespace parser {

struct NumberLocation;
struct PatternKey;

const NumberLocation* chooseNumberLocation(const PatternKey& forPattern);

} /* namespace parser */
} /* namespace sequence */

#endif /* DEFAULTNUMBERLOCATIONCHOOSER_H_ */
