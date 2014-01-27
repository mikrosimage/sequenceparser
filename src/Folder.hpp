#ifndef _SEQUENCE_PARSER_FOLDER_HPP_
#define _SEQUENCE_PARSER_FOLDER_HPP_

#include <FileObject.hpp>

namespace sequenceParser {

class Folder : public FileObject
{
public:
	Folder()
	{}
	
	Folder( const boost::filesystem::path& directory, const std::string folderName, const EMaskOptions& options )
	: FileObject( directory, eMaskTypeDirectory, options )
	, _folderName( folderName )
	{}

	Folder( const Folder& other )
	: FileObject( other )
	, _folderName( other._folderName )
	{}
	
	~Folder() {}

	std::ostream& getCout( std::ostream& os ) const;

	std::vector<boost::filesystem::path> getFiles() const;

	inline void clear()
	{
		FileObject::clear();
		_folderName.clear();
	}

	Folder* clone() const { return new Folder(*this); }
	
private:
	std::ostream& getProperties( std::ostream& os, const boost::filesystem::path& directory );

private:
	std::string _folderName;
};

}

#endif
