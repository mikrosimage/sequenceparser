#ifndef _SEQUENCE_PARSER_FILE_STRINGS_HPP_
#define _SEQUENCE_PARSER_FILE_STRINGS_HPP_

#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <vector>

namespace bfs = boost::filesystem;

namespace sequenceParser {

/// Internal structures to detect sequence inside a directory

/**
 * @brief Unique identification for a sequence
 */
class FileStrings {

public:
    typedef FileStrings This;
    typedef std::vector<std::string> Vec;

public:
    Vec& getId() {
	return _id;
    }

    const Vec& getId() const {
	return _id;
    }

    void clear() {
	_id.clear();
    }

    bool operator==(const This& v) const {
	if (_id.size() != v._id.size()) {
	    return false;
	}
	for (Vec::const_iterator i = _id.begin(), iEnd = _id.end(), vi = v._id.begin(); i != iEnd; ++i, ++vi) {
	    if (*i != *vi) {
		return false;
	    }
	}
	return true;
    }

    const std::string& operator[](const std::size_t i) const {
	return _id[i];
    }

    std::size_t getHash() const {
	std::size_t seed = 0;
	BOOST_FOREACH( const Vec::value_type & i, _id )
	{
	    boost::hash_combine(seed, i);
	    boost::hash_combine(seed, 1); // not like the hash of the concatenation of _id
	}
	return seed;
    }

    friend std::ostream& operator<<(std::ostream& os, const This& p);

private:
    Vec _id;
};


}
#endif
