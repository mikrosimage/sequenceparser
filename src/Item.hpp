#ifndef _SEQUENCE_PARSER_ITEM_HPP_
#define _SEQUENCE_PARSER_ITEM_HPP_

#include "common.hpp"
#include "Sequence.hpp"

#include <boost/filesystem/path.hpp>

#ifdef SWIGJAVA
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp> 
#include <boost/iostreams/code_converter.hpp>
#include <boost/locale.hpp>
#endif

namespace sequenceParser {


#ifdef SWIGJAVA
std::string utf8_to_latin1( const std::string& utf8_path )
{
	using namespace boost::locale::conv;
	std::string latin1_path = from_utf<char>(utf8_path, "Latin1");
	return latin1_path;
}
#endif


class Item
{
public:
	Item()
	: _type(eTypeFile)
	{}
	
	Item( const EType type, const std::string& filepath )
	: _type(type)
#ifdef SWIGJAVA
	// Strings are retrieved from JNI using GetStringUTFChars.
	// So there is an implicit UTF8 conversion.
	, _path(utf8_to_latin1(filepath))
#else
	, _path(filepath)
#endif
	{
	}

	Item( const EType type, const boost::filesystem::path& filepath )
	: _type(type)
	, _path(filepath)
	{
		BOOST_ASSERT( type != eTypeSequence );
	}
	
	Item( const Sequence& sequence, const boost::filesystem::path& folder )
	: _type(eTypeSequence)
	, _path(folder)
	, _sequence(sequence)
	{
		_path /= sequence.getStandardPattern();
	}

	Item( const Sequence& sequence, const std::string& folder )
	: _type(eTypeSequence)
	, _path(folder)
	, _sequence(sequence)
	{
		_path /= sequence.getStandardPattern();
	}

	EType getType() const { return _type; }

	std::string getAbsoluteFilepath() const { return _path.string(); }
	std::string getFilename() const { return _path.filename().string(); }
	std::string getFolder() const { return getFolderPath().string(); }

	std::vector<Item> explode();

	const Sequence& getSequence() const { return _sequence; }

	const boost::filesystem::path& getPath() const { return _path; }
	const boost::filesystem::path getFolderPath() const { return _path.parent_path(); }

	std::string getAbsoluteFirstFilename() const;
	std::string getFirstFilename() const;
	
	bool operator<( const Item& other ) const
	{
		return _path < other._path;
	}

	std::string string() const { return getType() == eTypeSequence ? getSequence().string() : _path.string(); }

private:
	EType _type;
	
	boost::filesystem::path _path;

	Sequence _sequence;
};


EType getTypeFromPath( const boost::filesystem::path& path );


std::ostream& operator<<( std::ostream& os, const Item& item );


}

#endif
