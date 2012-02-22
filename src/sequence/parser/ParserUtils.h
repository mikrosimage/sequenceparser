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
#include <boost/unordered_map.hpp>
#include <boost/container/flat_set.hpp>

#include <set>
#include <map>
#include <string>

namespace sequence {
namespace parser {
namespace details {

/**
 * The type used to store values found in the pattern
 */
typedef unsigned int value_type;

/**
 * A set of numbers found in filenames
 */
typedef std::vector<value_type> Values;

/**
 * A location within a string
 */
struct Location {
    unsigned char first;
    unsigned char count;
    Location() {}
    Location(unsigned char first, unsigned char count) : first(first), count(count) { }
};

/**
 * A set of locations
 */
typedef std::vector<Location> Locations;

/**
 * Extract all the numbers in a filename and their respective locations.
 * 'filename' is then set to the actual pattern with all its number replaced
 * by some #.
 * eg : file-20.1234.cr2
 * will produce the following locations : [5,2],[8,4],[15,1]
 * will produce the following number    : 20,1234,2
 * will produce the following pattern   : file-##.####.cr#
 */
void extractPattern(std::string &filename, Locations &locations, Values &numbers);

/**
 * Represents the different values for a given Location and a given Pattern.
 *
 * Consider the following sequence : file-1.jp2k, file-2.jp2k, file-3.jp2k, file-4.jp2k
 * The extracted pattern will be : file-#.jp#k
 *
 * The first LocationValueSet will contain : 1,2,3,4
 * The second LocationValueSet will contain : 2
 */
struct LocationValueSet : public std::set<value_type> {
    /**
     * returns true if this set contains a single value
     */
    bool isConstant() const { return size() == 1; }
    /**
     * returns the value if this set is a constant
     */
    value_type getConstantValue() const { return *begin(); }
    /**
     * retrieves all the consecutive Range found in this set
     * ie : 5,6,7,10,11,12,21 will produce
     * [5,7] [10,12] [21,21]
     */
    std::vector<Range> getConsecutiveRanges(value_type &step) const;
};

/**
 * This object is responsible for aggregating the values for a single
 * identified pattern ( eg : file-####.jpg )
 *
 * It stores all the values collected during parsing and insert them into the
 * corresponding LocationValueSet.
 *
 * After all the values are collected we can make informed decision about which
 * Location is actually the counter, the other ones are then discarded by mutating
 * this object through the removeConstantLocation function.
 */
struct PatternAggregator : public Values {
    typedef std::vector<LocationValueSet> LocationValueSets;
    PatternAggregator(const std::string &key, const Locations& locations) : key(key), locations(locations), locationValueSets(locations.size()) { }
    /**
     * append a set of values for a new entry
     */
    void append(const Values &values);
    /**
     * returns the number of filenames within this pattern
     */
    size_t fileCount() const { return locations.empty() ? 1 : size() / locationCount(); }
    /**
     * returns the number of Location within this pattern
     */
    size_t locationCount() const { return locations.size(); }
    /**
     * false if more than one Location is available
     */
    inline bool isReady() const { return locationCount() < 2; }
    /**
     * if no other value was appended, this pattern is a single file with numbers in it
     */
    inline bool isSingleFile() const { return fileCount() == 1; }
    /**
     * returns the Set selected as a counter for this pattern
     */
    const LocationValueSet& getSelectedSet() const { assert(locationCount()==1); return locationValueSets[0]; }
    /**
     * creates a copy of this pattern and bake the constant values in the key
     */
    PatternAggregator removeConstantLocations() const;
    /**
     * testing purpose only, returns the sets associated to Locations
     */
    const LocationValueSets& getValueSets() const { return locationValueSets; }
public:
    std::string key;
    Locations locations;
private:
    PatternAggregator discard(const std::vector<bool> &) const;
private:
    LocationValueSets locationValueSets;
};

/**
 * The core of the pattern detector.
 * Calling operator() will extract the pattern, the corresponding locations and values.
 * Values will be grouped by pattern and accumulated in a set per Location.
 *
 * Calling getResult() will
 * - remove constant LocationValueSet from pattern if needed
 * - extract contiguous Ranges from the only Location
 * - generate the according BrowserItem
 * - return the list of BrowseItem
 */
struct SequenceDetector : private boost::unordered_map<std::string, PatternAggregator> {
    SequenceDetector(const boost::filesystem::path &path = "") : path(path) { }
    const PatternAggregator& operator()(std::string filename);
    const std::vector<BrowseItem>& getResults();
private:
    void process(const value_type&);
    void reduce();
    boost::filesystem::path path;
    Values extractedValues;
    Locations extractedLocations;
    std::vector<BrowseItem> results;
    typedef boost::unordered_map<std::string, PatternAggregator> ME;
public:
    using ME::begin;
    using ME::end;
    using ME::value_type;
};

struct Parser : private boost::unordered_map<boost::filesystem::path, SequenceDetector>, private boost::noncopyable{
    void operator()(const boost::filesystem::path &);
    std::vector<BrowseItem> getResults();
    struct Proxy {
        Proxy(Parser *ptr) : ptr(ptr) {}
        inline void operator()(const boost::filesystem::path &path){ (*ptr)(path); }
        Parser *ptr;
    };
    Proxy functor(){ return Proxy(this); }
};

} /* namespace details */
} /* namespace parser */
} /* namespace sequence */
#endif /* PARSERUTILS_H_ */
