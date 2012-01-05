#ifndef _FILE_NUMBERS_HPP_
#define _FILE_NUMBERS_HPP_

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <set>

#include <commonDefinitions.hpp>

namespace sequenceParser
{

/// Internal structures to detect sequence inside a directory
/**
 * @brief Numbers inside a filename.
 * Each number can be a time inside a sequence.
 */
class FileNumbers
{

public:
    typedef FileNumbers                  This;
    typedef std::pair<Time, std::string> Pair;
    typedef std::vector<Pair>            Vec;

public:
    FileNumbers()
    {
	// we preverse reserve and take memory,
	// that realloc and takes time.
	_numbers.reserve(10);
    }

public:
    void push_back(const std::string& s)
    {
	Time t;
	try
	{
	    t = boost::lexical_cast<Time>(s);
	}
	catch (...)
	{
	    // can't retrieve the number,
	    // the number inside the string is probably
	    // ouf of range for Time type.
	    t = 0;
	}
	_numbers.push_back( Pair( t, s ) );
    }

    void clear()
    {
	_numbers.clear();
    }

    const std::string& getString(const std::size_t& i) const
    {
	return _numbers[i].second;
    }

    std::size_t getPadding(const std::size_t& i) const
    {
	const std::string& s = _numbers[i].second;

	if (s.size() == 1)
	    return 0;
	return s[0] == '0' ? s.size() : 0;
    }

    Time getTime(const std::size_t& i) const
    {
	return _numbers[i].first;
    }

    std::size_t size() const
    {
	return _numbers.size();
    }

    bool operator<(const This& v)
    {
	// can't have multiple size, if multiple size they must have a
	// different SeqId
	BOOST_ASSERT( _numbers.size() == v._numbers.size() );
	for (Vec::const_iterator i = _numbers.begin(), iEnd = _numbers.end(), vi = v._numbers.begin(); i != iEnd; ++i, ++vi)
	{
	    if (i->first < vi->first)
		return true;
	    else if (i->first > vi->first)
		return false;
	}
	return false; // equals
    }

    bool rangeEquals( const This& v, const size_t begin, const size_t end ) const
    {
	for (size_t i = begin; i < end; ++i)
	{
	    const Pair& me = this->_numbers[i];
	    const Pair& other = v._numbers[i];

	    //me.second.size() != other.second.size() ) // we don't check the padding...
	    if (me.first != other.first)
		return false;
	}
	return true;
    }

    friend std::ostream& operator<<( std::ostream& os, const This& p );

private:
    Vec _numbers;
};

}
#endif
