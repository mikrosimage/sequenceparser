#ifndef _SEQUENCE_PARSER_FILE_STRINGS_HPP_
#define _SEQUENCE_PARSER_FILE_STRINGS_HPP_

#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <vector>

namespace sequenceParser {
namespace detail {

/**
 * @brief Unique identification for a sequence.
 * Internal structures to detect sequence inside a directory.
 */
class FileStrings
{

public:
	typedef FileStrings This;
	typedef std::vector<std::string> Vec;

public:

	Vec& getId()
	{
		return _id;
	}

	const Vec& getId() const
	{
		return _id;
	}

	void clear()
	{
		_id.clear();
	}

	bool operator==( const This& v ) const
	{
		if( _id.size() != v._id.size() )
		{
			return false;
		}
		for( Vec::const_iterator i = _id.begin(), iEnd = _id.end(), vi = v._id.begin(); i != iEnd; ++i, ++vi )
		{
			if( *i != *vi )
			{
				return false;
			}
		}
		return true;
	}

	const std::string& operator[]( const std::size_t i ) const
	{
		return _id[i];
	}

	std::size_t getHash() const;

	friend std::ostream& operator<<( std::ostream& os, const This& p );

private:
	Vec _id;
};

// NOTE How we can replace this with a wrapper?
// Like boost::function, boost::bind,...
struct SeqIdHash : std::unary_function<FileStrings, std::size_t>
{
	std::size_t operator()(const FileStrings & p ) const
	{
		return p.getHash();
	}
};

}
}

#endif
