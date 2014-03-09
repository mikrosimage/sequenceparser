#ifndef _SEQUENCE_PARSER_FILE_HPP_
#define _SEQUENCE_PARSER_FILE_HPP_

#include <FileObject.hpp>

namespace sequenceParser {

class File : public FileObject
{
public:
	File()
	{}
	
	File( const boost::filesystem::path& directory, const std::string& filename, const EDisplay& displayOptions )
	: FileObject( directory, eTypeFile, displayOptions )
	, _filename( filename )
	{}

	File( const File& other )
	: FileObject( other )
	, _filename( other._filename )
	{}
	
	~File() { }

public:
	std::ostream& getCout( std::ostream& os ) const;

	std::vector<boost::filesystem::path> getFiles() const;

	inline std::string getAbsoluteFilename() const
	{
		return ( getAbsoluteDirectory() / _filename ).string();
	}

	inline std::string getFilename() const
	{
		return _filename;
	}

	inline void clear()
	{
		FileObject::clear();
		_filename.clear();
	}
	
	File* clone() const { return new File(*this); }

private:
	std::string _filename;
};

}

#endif

