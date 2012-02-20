/*
 * ParserUtils.h
 *
 *  Created on: 16 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PARSERUTILS_H_
#define PARSERUTILS_H_

#include <sequence/BrowseItem.h>

#include <boost/filesystem.hpp>

#include <set>
#include <map>
#include <string>

namespace sequence {
namespace parser {
namespace details {

struct Location {
    unsigned char first;
    unsigned char count;
    Location() {
    }
    Location(unsigned char first, unsigned char count) :
                    first(first), count(count) {
    }
};

typedef std::vector<unsigned int> Values;
typedef std::vector<Location> Locations;

void extractPattern(std::string &pattern, Locations &locations, Values &numbers);

struct LocationValueSet : public std::set<unsigned int> {
    bool isConstant() const {
        return size() == 1;
    }
    value_type getConstantValue() const {
        return *begin();
    }
    std::vector<Range> getRanges(value_type &step) const;
};

struct PatternAggregator : public Values {
    typedef std::vector<LocationValueSet> LocationValueSets;

    PatternAggregator(const std::string &key, const Locations& locations) :
                    key(key), locations(locations), locationValueSets(locations.size()) {
    }

    void append(const Values &values);
    size_t fileCount() const {
        return locations.empty() ? 1 : size() / locationCount();
    }
    size_t locationCount() const {
        return locations.size();
    }
    inline bool isValid() const {
        return isSingleFile() || locationCount() == 1;
    }
    inline bool isSingleFile() const {
        return fileCount() == 1;
    }
    const LocationValueSets& getSets() const {
        return locationValueSets;
    }
    const LocationValueSet& getSelectedSet() const {
        assert(locationCount()==1);
        return locationValueSets[0];
    }
    PatternAggregator removeConstantLocations() const;
public:
    std::string key;
    Locations locations;
private:
    PatternAggregator createCopyWithLocations(const std::vector<bool> &) const;
private:
    LocationValueSets locationValueSets;
};

struct SequenceDetector : private std::map<std::string, PatternAggregator> {
    SequenceDetector(const boost::filesystem::path &path = "") :
                    path(path) {
    }
    const PatternAggregator& operator()(std::string filename);
    const std::vector<BrowseItem>& getResults();
private:
    void process(const value_type&);
    void reduce();
    boost::filesystem::path path;
    Values extractedValues;
    Locations extractedLocations;
    std::vector<BrowseItem> results;
    typedef std::map<std::string, PatternAggregator> ME;
public:
    using ME::begin;
    using ME::end;
    using ME::value_type;
};

} /* namespace details */
} /* namespace parser */
} /* namespace sequence */
#endif /* PARSERUTILS_H_ */
