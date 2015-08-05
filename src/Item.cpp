#include "Item.hpp"

#include <boost/filesystem.hpp>


namespace bfs = boost::filesystem;

namespace sequenceParser {


std::string Item::getAbsoluteFirstFilename() const
{
	if( getType() == eTypeSequence )
		return (_path.parent_path() / getSequence().getFirstFilename()).string();
	return getAbsoluteFilepath();
}


std::vector<Item> Item::explode() const{
	std::vector<Item> outItems;

	if(_type != eTypeSequence){
		outItems.push_back(*this);
		return outItems;
	}

	const std::vector<boost::filesystem::path>& seqFilesPath = getSequence().getAbsoluteFilesPath(_path.parent_path());
	BOOST_FOREACH(const boost::filesystem::path& filePath, seqFilesPath)
		outItems.push_back(Item(getTypeFromPath(filePath), filePath));
	return outItems;
}

std::string Item::getFirstFilename() const
{
	if( getType() == eTypeSequence )
		return getSequence().getFirstFilename();
	return getFilename();
}

EType getTypeFromPath( const boost::filesystem::path& path )
{
	if( bfs::is_symlink( path ) )
	{
		return eTypeLink;
	}
	if( bfs::is_regular_file( path ) )
	{
		return eTypeFile;
	}
	if( bfs::is_directory( path ) )
	{
		return eTypeFolder;
	}
	return eTypeUndefined;
}


EType getTypeFromPath( const std::string& pathStr )
{
	const boost::filesystem::path path( pathStr );
	return getTypeFromPath(path);
}


std::ostream& operator<<( std::ostream& os, const Item& item )
{
	os << (item.getType() == eTypeSequence ? (item.getFolderPath() / item.getSequence().string()) : item.getPath());
	return os;
}


}
