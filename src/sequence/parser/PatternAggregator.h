/*
 * PatternAggregator.h
 *
 *  Created on: Feb 16, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PATTERNAGGREGATOR_H_
#define PATTERNAGGREGATOR_H_

#include <boost/filesystem/path.hpp>

#include <set>
#include <map>
#include <string>

namespace sequence {
namespace parser {

/**
 * A location in the filename assuming filename length is less then 256
 */
struct Location {
    unsigned char first;
    unsigned char count;
    Location() {
    }
    Location(unsigned char first, unsigned char count) :
                    first(first), count(count) {
    }
};

/**
 * Custom vector<Location> implementation
 * to avoid dynamic allocation and maximize
 * locality
 */
struct Locations {
    Locations();
    void push_back(const Location& location);
    size_t size() const;
    const Location& operator[](size_t index) const;
private:
    static const unsigned char MAX_LOCATIONS = 4;
    Location locations[MAX_LOCATIONS];
    unsigned char count;
};

struct PatternKey {
    std::string key;
    Locations locations;
    std::vector<unsigned int> values;
    PatternKey(const std::string& filename);
    size_t size() const { return locations.size(); }
};

struct PatternAggregator {
    PatternAggregator();

    void operator()(const boost::filesystem::path& path);

    typedef std::map<std::string, PatternKey> KeyMap;
    typedef std::map<boost::filesystem::path,KeyMap> FolderMap;
    FolderMap folderMap;
};

} /* namespace parser */
} /* namespace sequence */
#endif /* PATTERNAGGREGATOR_H_ */
