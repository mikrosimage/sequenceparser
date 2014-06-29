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
	, _folder(folder)
	, _filename(filename)
	{
		assert( type != eTypeSequence );
	}
	
	Item( const Sequence& sequence, const std::string& folder )
	: _type(eTypeSequence)
	, _folder(folder)
	, _filename(sequence.getStandardPattern())
	, _sequence(sequence)
	{}
	
	EType getType() const { return _type; }
	const std::string& getFolder() const { return _folder; }
	const std::string& getFilename() const { return _filename; }
	boost::filesystem::path getAbsPath() const
	{
		return boost::filesystem::path(_folder) / _filename;
	}
	std::string getAbsPathStr() const { return getAbsPath().string(); }
	
	bool operator<( const Item& other ) const
	{
		if( _folder == other._folder )
			return _filename < other._filename;
		return _folder < other._folder;
	}

//	ItemStat getStat( bool approximative = True )

private:
	EType _type;
	
	std::string _folder;
	std::string _filename;
	Sequence _sequence;
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
