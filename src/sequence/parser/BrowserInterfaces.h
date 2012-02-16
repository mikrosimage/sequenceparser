/*
 * BrowserInterfaces.h
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef BROWSERINTERFACES_H_
#define BROWSERINTERFACES_H_

#include "PatternAggregator.h"

#include <sequence/BrowseItem.h>

#include <boost/function.hpp>

namespace sequence {
namespace parser {

/**
 * Function called to select which location will be used as a counter within
 * the pattern. If the pattern is suspected to be file/folder just return NULL.
 */
typedef boost::function<const NumberLocation*(const PatternKey&)> ChooseNumberLocation;

/**
 * Takes a pattern and generates a vector of BrowseItem from it
 */
typedef boost::function<std::vector<BrowseItem>(const PatternKey&)> GenerateItems;

} /* namespace parser */
} /* namespace sequence */
#endif /* BROWSERINTERFACES_H_ */
