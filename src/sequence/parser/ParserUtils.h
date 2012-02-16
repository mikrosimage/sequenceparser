/*
 * ParserUtils.h
 *
 *  Created on: 16 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PARSERUTILS_H_
#define PARSERUTILS_H_


#include <boost/filesystem/path.hpp>

#include <set>
#include <map>
#include <string>

namespace sequence {
namespace parser {

struct NumberLocation {
    unsigned char first;
    unsigned char count;
    NumberLocation() { }
    NumberLocation(unsigned char first, unsigned char count) : first(first), count(count) { }
};

struct NumberLocationInstance {
    NumberLocation location;
    unsigned int value;
};

typedef std::vector<unsigned int> Values;
typedef std::set<unsigned int> ValueSet;
typedef std::vector<NumberLocation> NumberLocations;
typedef std::vector<NumberLocationInstance> NumberLocationInstances;

void extractPattern(const std::string &filename, std::string &pattern, NumberLocationInstances &numbers);



struct PatternKey {
    std::string key;
    std::vector<NumberLocation> numberLocations;
    PatternKey(const std::string& key, const NumberLocations &locations);
    bool operator<(const PatternKey&other)const{return key<other.key;}
    bool operator==(const PatternKey&other)const{return key==other.key;}
};

typedef std::map<PatternKey, Values> KeyMap;

typedef std::map<boost::filesystem::path, KeyMap> FolderMap;

struct PatternAggregator {
    void operator()(const boost::filesystem::path& path);
    const FolderMap& getFolderMap() const { return folderMap; }
private:
    FolderMap folderMap;
};

} /* namespace parser */
} /* namespace sequence */
#endif /* PARSERUTILS_H_ */
