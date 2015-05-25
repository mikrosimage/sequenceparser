#ifndef _SEQUENCE_PARSER_ITEMSTAT_HPP_
#define _SEQUENCE_PARSER_ITEMSTAT_HPP_

#include "Item.hpp"
#include "common.hpp"
#include "system.hpp"

#ifdef __UNIX__
    #include <sys/types.h>
#endif


namespace sequenceParser {

class ItemStat
{
public:
	ItemStat( const Item& item, const bool approximative=true );
	ItemStat( const EType& type, const boost::filesystem::path& path, const bool approximative=true );

	std::string getUserName() const;
	std::string getGroupName() const;

private:
	void statFolder( const boost::filesystem::path& path );
	void statFile( const boost::filesystem::path& path );
	void statSequence( const Item& item, const bool approximative );
	void statLink( const boost::filesystem::path& path );
	void setDefaultValues();
#ifdef __UNIX__
	void setPermissions( const mode_t& protection );
#endif

public:
	long long deviceId;
	unsigned int inodeId;
	double nbHardLinks;
	long long fullNbHardLinks;
	long long userId;
	long long groupId;
	long long size;
	long long minSize; /// size of the smallest file in the sequence (otherwise, same value as size)
	long long maxSize; /// size of the biggest file in the sequence (otherwise, same value as size)
	long long realSize; /// size (takes hardlinks into account)
	long long sizeOnDisk; /// size on hard-drive (takes hardlinks into account)
	long long accessTime;
	long long modificationTime;
	long long creationTime;
	bool ownerCanRead;
	bool ownerCanWrite;
	bool ownerCanExecute;
	bool groupCanRead;
	bool groupCanWrite;
	bool groupCanExecute;
	bool otherCanRead;
	bool otherCanWrite;
	bool otherCanExecute;
};

}

#endif
