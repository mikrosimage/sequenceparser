#ifndef _SEQUENCE_PARSER_FILESYSTEM_HPP_
#define _SEQUENCE_PARSER_FILESYSTEM_HPP_

#include "commonDefinitions.hpp"
#include "Sequence.hpp"
#include "File.hpp"
#include "Folder.hpp"

#include <iostream>
#include <iomanip>

namespace sequenceParser {

enum EType {
	eTypeFolder,
	eTypeSequence,
	eTypeFile
};

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
	, _filename(sequence.getIdentification())
	, _sequence(sequence)
	{}
	
	EType _type;
	
	std::string _folder;
	std::string _filename;
	Sequence _sequence;

//	std::string& getFilepath();
//	std::string& getFirstFilepath();
//	std::string& getFirstFilename();
//	stat(approximative=True)
//	operator<(item)
};

std::vector<Item> browse(
	const std::string& directory, std::vector<std::string>& filters, const EMaskOptions desc = eMaskOptionsDefault );

inline std::vector<Item> browse(
	const std::string& directory, const EMaskOptions desc = eMaskOptionsDefault )
{
	std::vector<std::string> filters;
	return browse( directory, filters, desc );
}

////#ifndef SWIG
//void browse(
//	std::vector<Folder>& outFolders, std::vector<Sequence>& outSequences, std::vector<File>& outFiles,
//	const std::string& directory, std::vector<std::string>& filters, const EMaskOptions desc = eMaskOptionsDefault );
////#endif
}

#endif
