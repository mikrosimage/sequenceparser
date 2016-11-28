#include "ItemStat.hpp"

#include <boost/filesystem/operations.hpp>

#ifdef __UNIX__
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#endif


namespace bfs = boost::filesystem;

namespace sequenceParser {

ItemStat::ItemStat( const Item& item, const bool approximative )
	: deviceId(0)
	, inodeId(0)
	, nbHardLinks(0)
	, fullNbHardLinks(0)
	, userId(0)
	, groupId(0)
	, userName("unknown")
	, groupName("unknown")
	, size(0)
	, minSize(0)
	, maxSize(0)
	, realSize(0)
	, sizeOnDisk(0)
	, accessTime(-1)
	, modificationTime(-1)
	, lastChangeTime(-1)
	, ownerCanRead(false)
	, ownerCanWrite(false)
	, ownerCanExecute(false)
	, groupCanRead(false)
	, groupCanWrite(false)
	, groupCanExecute(false)
	, otherCanRead(false)
	, otherCanWrite(false)
	, otherCanExecute(false)
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
		default:
			break;
	}
}

#ifdef __UNIX__

void ItemStat::setPermissions( const mode_t& protection )
{
	ownerCanRead = protection & S_IRUSR;
	ownerCanWrite = protection & S_IWUSR;
	ownerCanExecute = protection & S_IXUSR;
	groupCanRead = protection & S_IRGRP;
	groupCanWrite = protection & S_IWGRP;
	groupCanExecute = protection & S_IXGRP;
	otherCanRead = protection & S_IROTH;
	otherCanWrite = protection & S_IWOTH;
	otherCanExecute = protection & S_IXOTH;
}

void ItemStat::setUserName()
{
    passwd* user = getpwuid(userId);
    if(user && user->pw_name)
        userName = std::string(user->pw_name);
}

void ItemStat::setGroupName()
{
    group* group = getgrgid(groupId);
    if(group && group->gr_name)
        groupName = std::string(group->gr_name);
}

#endif

void ItemStat::statLink( const boost::filesystem::path& path )
{
	boost::system::error_code errorCode;
	const long long last_write_time = bfs::last_write_time(path, errorCode);
	if(errorCode == boost::system::errc::success)
	{
		modificationTime = last_write_time;
	}

#ifdef __UNIX__
	struct stat statInfos;
	const int statStatus = lstat(path.c_str(), &statInfos);
	if (statStatus == -1)
		return;

	fullNbHardLinks = nbHardLinks = statInfos.st_nlink;
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	lastChangeTime = statInfos.st_ctime;
	size = statInfos.st_size;
	minSize = size;
	maxSize = size;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * statInfos.st_blksize;
	setPermissions(statInfos.st_mode);
	setUserName();
	setGroupName();
#else
	fullNbHardLinks = nbHardLinks = 1;
#endif
	realSize = size / nbHardLinks;
}

void ItemStat::statFolder( const boost::filesystem::path& path )
{
	boost::system::error_code errorCode;
	const size_t hard_link_count = bfs::hard_link_count(path, errorCode);
	if(errorCode == boost::system::errc::success)
	{
		fullNbHardLinks = nbHardLinks = hard_link_count;
		modificationTime = bfs::last_write_time(path, errorCode);
	}

#ifdef __UNIX__
	struct stat statInfos;
	const int statStatus = lstat(path.c_str(), &statInfos);
	if (statStatus == -1)
		return;

	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	lastChangeTime = statInfos.st_ctime;
	size = statInfos.st_size;
	minSize = size;
	maxSize = size;
	sizeOnDisk = statInfos.st_blocks * statInfos.st_blksize;
	setPermissions(statInfos.st_mode);
	setUserName();
	setGroupName();
#endif

	// size (takes hardlinks into account)
	realSize = size;
}

void ItemStat::statFile( const boost::filesystem::path& path )
{
	boost::system::error_code errorCode;
	const size_t hard_link_count = bfs::hard_link_count(path, errorCode);
	if(errorCode == boost::system::errc::success)
	{
		fullNbHardLinks = nbHardLinks = hard_link_count;
		size = bfs::file_size(path, errorCode);
		minSize = size;
		maxSize = size;
		modificationTime = bfs::last_write_time(path, errorCode);
	}

#ifdef __UNIX__
	struct stat statInfos;
	const int statStatus = lstat(path.c_str(), &statInfos);
	if (statStatus == -1)
		return;

	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	lastChangeTime = statInfos.st_ctime;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * statInfos.st_blksize;
	setPermissions(statInfos.st_mode);
	setUserName();
	setGroupName();
#endif

	// size (takes hardlinks into account)
	realSize = size / nbHardLinks;
}

void ItemStat::statSequence( const Item& item, const bool approximative )
{
#ifdef __UNIX__
	struct stat statInfos;
	const int statStatus = lstat(item.getAbsoluteFirstFilename().c_str(), &statInfos);
	if (statStatus == -1)
		return;

	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	setPermissions(statInfos.st_mode);
	setUserName();
	setGroupName();
#endif

	modificationTime = 0;
	fullNbHardLinks = 0;
	size = 0;
	minSize = 0;
	maxSize = 0;
	realSize = 0;
	sizeOnDisk = 0;
	lastChangeTime = 0;

	const Sequence& seq = item.getSequence();

	// if( ! approximative )
	// else
	//   TODO: stat on a subset of files

	BOOST_FOREACH(Item item, item.explode() )
	{
		const ItemStat fileStat(item);

		// use the most restrictive permissions in the sequence
#ifdef __UNIX__
		// user
		if( ownerCanRead && ! fileStat.ownerCanRead)
			ownerCanRead = false;
		if( ownerCanWrite && ! fileStat.ownerCanWrite)
			ownerCanWrite = false;
		if( ownerCanExecute && ! fileStat.ownerCanExecute)
			ownerCanExecute = false;
		// group
		if( groupCanRead && ! fileStat.groupCanRead)
			groupCanRead = false;
		if( groupCanWrite && ! fileStat.groupCanWrite)
			groupCanWrite = false;
		if( groupCanExecute && ! fileStat.groupCanExecute)
			groupCanExecute = false;
		// other
		if( otherCanRead && ! fileStat.otherCanRead)
			otherCanRead = false;
		if( otherCanWrite && ! fileStat.otherCanWrite)
			otherCanWrite = false;
		if( otherCanExecute && ! fileStat.otherCanExecute)
			otherCanExecute = false;
#endif

		// use the latest modification date in the sequence
		if( fileStat.modificationTime > modificationTime )
			modificationTime = fileStat.modificationTime;
		if( lastChangeTime == 0 || lastChangeTime < fileStat.lastChangeTime )
			lastChangeTime = fileStat.lastChangeTime;

		// compute sizes
		fullNbHardLinks += fileStat.fullNbHardLinks;
		size += fileStat.size;
		if( minSize == 0 || minSize > fileStat.size )
			minSize = fileStat.size;
		if( maxSize == 0 || maxSize < fileStat.size )
			maxSize = fileStat.size;
		realSize += fileStat.realSize;
		sizeOnDisk += fileStat.sizeOnDisk;
	}

	nbHardLinks = fullNbHardLinks / (double)(seq.getLastTime() - seq.getFirstTime() + 1);
}

}
