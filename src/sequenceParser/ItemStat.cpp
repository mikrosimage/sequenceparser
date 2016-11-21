#include "ItemStat.hpp"

#include <boost/filesystem/operations.hpp>

#ifdef __UNIX__
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#endif


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
			setDefaultValues();
			break;
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
			setDefaultValues();
			break;
	}
}

std::string ItemStat::getUserName() const
{
#ifdef __UNIX__
	passwd* user = getpwuid(userId);
	if(user == NULL)
		return std::string("unknown");
	return std::string(user->pw_name ? user->pw_name : "unknown");
#else
	return std::string("not implemented");
#endif
}

std::string ItemStat::getGroupName() const
{
#ifdef __UNIX__
	group* group = getgrgid(groupId);
	if(group == NULL)
		return std::string("unknown");
	return std::string(group->gr_name ? group->gr_name : "unknown");
#else
	return std::string("not implemented");
#endif
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
#endif

void ItemStat::statLink( const boost::filesystem::path& path )
{
	boost::system::error_code errorCode;
	modificationTime = bfs::last_write_time(path, errorCode);
	int stat_status = -1;

#ifdef __UNIX__
	struct stat statInfos;
	stat_status = lstat(path.c_str(), &statInfos);
	if (stat_status == -1)
	{
		setDefaultValues();
		return;
	}
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
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * 512;
	setPermissions(statInfos.st_mode);
#else
	fullNbHardLinks = nbHardLinks = 1;
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
	lastChangeTime = 0;
	sizeOnDisk = 0;
	size = 0;
#endif
	realSize = size / nbHardLinks;
}

void ItemStat::setDefaultValues(){
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
	lastChangeTime = -1;
	sizeOnDisk = 0;
	size = 0;
	minSize = 0;
	maxSize = 0;
	realSize = 0;
	fullNbHardLinks = 0;
	modificationTime = -1;
	ownerCanRead = false;
	ownerCanWrite = false;
	ownerCanExecute = false;
	groupCanRead = false;
	groupCanWrite = false;
	groupCanExecute = false;
	otherCanRead = false;
	otherCanWrite = false;
	otherCanExecute = false;
}

void ItemStat::statFolder( const boost::filesystem::path& path )
{
	using namespace boost::filesystem;
	boost::system::error_code errorCode;

	fullNbHardLinks = nbHardLinks = bfs::hard_link_count( path, errorCode );
	modificationTime = bfs::last_write_time( path, errorCode );
	int stat_status = -1;

#ifdef __UNIX__
	struct stat statInfos;
	stat_status = lstat( path.c_str(), &statInfos );
	if (stat_status == -1)
	{
		setDefaultValues();
		return;
	}
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
	sizeOnDisk = statInfos.st_blocks * 512;
	setPermissions(statInfos.st_mode);
#else
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
	lastChangeTime = 0;
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
	minSize = size;
	maxSize = size;
	modificationTime = bfs::last_write_time( path, errorCode );
	int stat_status = -1;

#ifdef __UNIX__
	struct stat statInfos;
	stat_status = lstat( path.c_str(), &statInfos );
	if (stat_status == -1)
	{
		setDefaultValues();
		return;
	}

	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	lastChangeTime = statInfos.st_ctime;
	// size on hard-drive (takes hardlinks into account)
	sizeOnDisk = (statInfos.st_blocks / nbHardLinks) * 512;
	setPermissions(statInfos.st_mode);
#else
	deviceId = 0;
	inodeId = 0;
	userId = 0;
	groupId = 0;
	accessTime = 0;
	lastChangeTime = 0;
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
	int stat_status = -1;

#ifdef __UNIX__
	struct stat statInfos;
	stat_status = lstat( item.getAbsoluteFirstFilename().c_str(), &statInfos );
	if (stat_status == -1)
	{
		setDefaultValues();
		return;
	}
	deviceId = statInfos.st_dev;
	inodeId = statInfos.st_ino;
	userId = statInfos.st_uid;
	groupId = statInfos.st_gid;
	accessTime = statInfos.st_atime;
	setPermissions(statInfos.st_mode);
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
	minSize = 0;
	maxSize = 0;
	realSize = 0;
	sizeOnDisk = 0;
	lastChangeTime = 0;

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
