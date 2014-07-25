#ifndef _SEQUENCE_PARSER_FILESYSTEM_HPP_
#define _SEQUENCE_PARSER_FILESYSTEM_HPP_

#include "commonDefinitions.hpp"
#include "Sequence.hpp"
#include "File.hpp"
#include "Folder.hpp"

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <iomanip>


namespace sequenceParser {

class Item
{
public:
	Item()
	: _type(eTypeFile)
	{}
	
	Item( const EType type, const std::string& filename, const std::string& folder )
	: _type(type)
	, _path(folder)
	{
		_path /= filename;
		assert( type != eTypeSequence );
	}
	
	Item( const Sequence& sequence, const std::string& folder )
	: _type(eTypeSequence)
	, _path(folder)
	, _sequence(sequence)
	{
		_path /= sequence.getStandardPattern();
	}

	EType getType() const { return _type; }

	std::string getAbsFilepath() const;
	std::string getFilename() const;
	std::string getFolder() const;

	const Sequence& getSequence() const { return _sequence; }

	const boost::filesystem::path& getPath() const { return _path; }

	std::string getAbsoluteFirstFilename() const;
	std::string getFirstFilename() const;
	
	bool operator<( const Item& other ) const
	{
		if( _folder == other._folder )
			return _filename < other._filename;
		return _folder < other._folder;
	}

private:
	EType _type;
	
	boost::filesystem::path _path;

	Sequence _sequence;
};


class ItemStat
{
public:
	ItemStat( const Item& item, const bool approximative=true );

private:
	void statFile( const Item& item );
	void statSequence( const Item& item, const bool approximative );

public:
	long long _deviceId;
	unsigned int _inodeId;
	double _nbHardLinks;
	long long _fullNbHardLinks;
	long long _userId;
	long long _groupId;
	long long _size;
	long long _realSize; /// size (takes hardlinks into account)
	long long _sizeOnDisk; /// size on hard-drive (takes hardlinks into account)
	long long _accessTime;
	long long _modificationTime;
	long long _creationTime;
};



std::vector<Item> browse(
		const std::string& directory,
		const std::vector<std::string>& filters,
		const EDetection detectOptions = eDetectionDefault,
		const EDisplay displayOptions = eDisplayDefault );

inline std::vector<Item> browse(
		const std::string& directory,
		const EDetection detectOptions = eDetectionDefault,
		const EDisplay displayOptions = eDisplayDefault )
{
	std::vector<std::string> filters;
	return browse( directory, filters, detectOptions, displayOptions );
}

////#ifndef SWIG
//void browse(
//	std::vector<Folder>& outFolders, std::vector<Sequence>& outSequences, std::vector<File>& outFiles,
//	const std::string& directory, std::vector<std::string>& filters, const EDetection detectOptions = eDetectionDefault );
////#endif
}

#endif
