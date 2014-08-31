#include "ItemStat.hpp"

#include <boost/filesystem/operations.hpp>

#include <sys/types.h>
#include <sys/stat.h>


namespace bfs = boost::filesystem;

namespace sequenceParser {

ItemStat::ItemStat( const EType& type, const boost::filesystem::path& path, const bool approximative )
{
	switch(type)
	{
		case eTypeFolder:
		{
			statFolder(path);
			break;
		}
		case eTypeFile:
		{
			statFile(path);
			break;
		}
		case eTypeLink:
		{
			statLink(path);
			break;
		}
		case eTypeUndefined:
		case eTypeSequence:
			BOOST_ASSERT(false);
	}
}

ItemStat::ItemStat( const Item& item, const bool approximative )
{
	switch(item.getType())
	{
		case eTypeFolder:
		{
			statFolder(item.getPath());
			break;
		}
		case eTypeFile:
		{
			statFile(item.getPath());
			break;
		}
		case eTypeLink:
		{
			statLink(item.getPath());
			break;
		}
		case eTypeSequence:
		{
			statSequence( item, approximative );
			break;
		}
		case eTypeUndefined:
			BOOST_ASSERT(false);
	}
}

void ItemStat::statLink( const boost::filesystem::path& path )
{
	boost::system::error_code errorCode;
	modificationTime = bfs::last_write_time(path, errorCode);

#ifdef UNIX
	struct stat statInfos;
	lstat(path.c_str(), &statInfos);
	fullNbHardLinks = nbHardLinks = statInfos.st_nlink;
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	creationTime = statInfos.st_ctime;
	size = statInfos.st_size;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * 512;
#else
	fullNbHardLinks = nbHardLinks = 1;
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime1 = 0;
	creationTime = 0;
	sizeOnDisk = 0;
	size = 0;
#endif
	realSize = size / nbHardLinks;
}

void ItemStat::statFolder( const boost::filesystem::path& path )
{
	using namespace boost::filesystem;
	boost::system::error_code errorCode;

	fullNbHardLinks = nbHardLinks = bfs::hard_link_count( path, errorCode );
	modificationTime = bfs::last_write_time( path, errorCode );

#ifdef UNIX
	struct stat statInfos;
	lstat( path.c_str(), &statInfos );
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	creationTime = statInfos.st_ctime;
	size = statInfos.st_size;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = statInfos.st_blocks * 512;
#else
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime1 = 0;
	creationTime = 0;
	sizeOnDisk = 0;
	size = 0;
#endif

	// size (takes hardlinks into account)
	realSize = size;
}

void ItemStat::statFile( const boost::filesystem::path& path )
{
	using namespace boost::filesystem;
	boost::system::error_code errorCode;
	fullNbHardLinks = nbHardLinks = bfs::hard_link_count( path, errorCode );
	size = bfs::file_size( path, errorCode );
	modificationTime = bfs::last_write_time( path, errorCode );

#ifdef UNIX
	struct stat statInfos;
	lstat( path.c_str(), &statInfos );
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	creationTime = statInfos.st_ctime;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * 512;
#else
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
	creationTime = 0;
	sizeOnDisk = 0;
#endif

	// size (takes hardlinks into account)
	realSize = size / nbHardLinks;
}

void ItemStat::statSequence( const Item& item, const bool approximative )
{
	using namespace boost::filesystem;
	using namespace sequenceParser;
	boost::system::error_code errorCode;

#ifdef UNIX
	struct stat statInfos;
	lstat( item.getAbsoluteFirstFilename().c_str(), &statInfos );
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
#else
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
#endif

	modificationTime = 0;
	fullNbHardLinks = 0;
	size = 0;
	realSize = 0;
	sizeOnDisk = 0;
	creationTime = 0;

	const Sequence& seq = item.getSequence();

	// if( ! approximative )
	// else
	//   TODO: stat on a subset of files

	bfs::path folder = item.getFolderPath();
	
	BOOST_FOREACH( Time t, seq.getFramesIterable() )
	{
		boost::filesystem::path filepath = folder / seq.getFilenameAt(t);

		EType type = getTypeFromPath(filepath);

		ItemStat fileStat(type, filepath);
		
		// use the latest modification date in the sequence
		if( fileStat.modificationTime > modificationTime )
			modificationTime = fileStat.modificationTime;
		if( creationTime == 0 || creationTime > fileStat.creationTime )
			creationTime = fileStat.creationTime;

		fullNbHardLinks += fileStat.fullNbHardLinks;
		size += fileStat.size;
		realSize += fileStat.realSize;
		sizeOnDisk += fileStat.sizeOnDisk;
	}

	nbHardLinks = fullNbHardLinks / (double)(seq.getLastTime() - seq.getFirstTime() + 1);
}

}
