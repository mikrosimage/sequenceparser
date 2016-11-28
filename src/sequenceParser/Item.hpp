#ifndef _SEQUENCE_PARSER_ITEM_HPP_
#define _SEQUENCE_PARSER_ITEM_HPP_

#include "common.hpp"
#include "Sequence.hpp"

#include <boost/filesystem/path.hpp>

namespace sequenceParser {

/**
 * @brief Represents an element in the filesystem: file, folder or link.
 * It could contain a Sequence.
 * @see getSequence
 */
class Item
{
public:
	Item()
	: _type(eTypeFile)
	{}
	
	Item( const EType type, const std::string& filepath )
	: _type(type)
#ifdef SWIGJAVA
	, _path(utf8_to_latin1(filepath))
#else
	, _path(filepath)
#endif
	{
	}

#ifndef SWIG
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
		_path /= sequence.getFilenameWithStandardPattern();
	}
#endif

	Item( const Sequence& sequence, const std::string& folder )
	: _type(eTypeSequence)
	, _path(folder)
	, _sequence(sequence)
	{
		_path /= sequence.getFilenameWithStandardPattern();
	}

	EType getType() const { return _type; }

	std::string getAbsoluteFilepath() const { return _path.string(); }
	std::string getFilename() const { return _path.filename().string(); }
	std::string getFolder() const { return getFolderPath().string(); }
	
	/**
	 * @brief Usefull for sequences items: explode sequence
	 */
	std::vector<Item> explode() const;

	const Sequence& getSequence() const { return _sequence; }

#ifndef SWIG
	const boost::filesystem::path& getPath() const { return _path; }
	const boost::filesystem::path getFolderPath() const { return _path.parent_path(); }
#endif

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


#ifndef SWIG
EType getTypeFromPath( const boost::filesystem::path& path );
#endif
/**
 * @warning The methods checks if the given path corresponds to a link, a file or a folder (in the filesystem).
 * Else the method returns unknown type (it never returns a sequence).
 * @see getTypeFromPath
 */
EType getTypeFromPath( const std::string& pathStr );

#ifndef SWIG
std::ostream& operator<<( std::ostream& os, const Item& item );
#endif

}

#endif
