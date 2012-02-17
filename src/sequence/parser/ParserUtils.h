/*
 * ParserUtils.h
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PARSERUTILS_H_
#define PARSERUTILS_H_

#include <sequence/BrowseItem.h>

#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>

#include <set>
#include <map>
#include <string>

namespace sequence {
namespace parser {

struct Location {
    unsigned char first;
    unsigned char count;
    Location() { }
    Location(unsigned char first, unsigned char count) : first(first), count(count) { }
};

struct LocationWithValue {
    Location location;
    unsigned int value;
};

typedef std::vector<unsigned int> Values;
typedef std::vector<Location> Locations;

void extractPattern(std::string &pattern, Locations &locations, Values &numbers);

struct PatternAggregator : public Values {
    typedef std::set<unsigned int> Set;
    typedef std::vector<Set> Sets;

    PatternAggregator(const std::string &key, const Locations& locations) : key(key), locations(locations), sets(locations.size()) {}

    void append(const Values &values);
    size_t files() const{return locations.empty() ? 1 : size() / locationCount();}
    size_t locationCount() const{return locations.size();}
    inline bool isOptimized()const{return isSingleFile()||locationCount()==1;}
    inline bool isSingleFile()const{return files()==1;}
    const Sets& getSets()const{return sets;}
    PatternAggregator optimize() const;
public:
    std::string key;
    Locations locations;
private:
    Sets sets;
};

struct FilenameAggregator : boost::unordered_map<std::string, PatternAggregator> {
    const PatternAggregator& operator()(std::string filename);
    void optimize();
private:
    Values tmpValues;
    Locations tmpLocations;
};

typedef std::map<boost::filesystem::path, FilenameAggregator> FolderMap;

struct Parser {
    const PatternAggregator& operator()(const boost::filesystem::path& filename);
    const FolderMap& getFolderMap() const { return folderMap; }
    std::vector<BrowseItem> finalize() ;
private:
    FolderMap folderMap;
};

} /* namespace parser */
} /* namespace sequence */
#endif /* PARSERUTILS_H_ */
