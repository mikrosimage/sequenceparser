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


std::ostream& operator<<( std::ostream& os, const Item& item )
{
	os << (item.getType() == eTypeSequence ? (item.getFolderPath() / item.getSequence().string()) : item.getPath());
	return os;
}


}
