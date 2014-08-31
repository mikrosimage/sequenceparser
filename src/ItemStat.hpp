#ifndef _SEQUENCE_PARSER_ITEMSTAT_HPP_
#define _SEQUENCE_PARSER_ITEMSTAT_HPP_

#include "Item.hpp"
#include "common.hpp"


namespace sequenceParser {

class ItemStat
{
public:
	ItemStat( const Item& item, const bool approximative=true );
	ItemStat( const EType& type, const boost::filesystem::path& path, const bool approximative=true );

private:
	void statFolder( const boost::filesystem::path& path );
	void statFile( const boost::filesystem::path& path );
	void statSequence( const Item& item, const bool approximative );
	void statLink( const boost::filesystem::path& path );

public:
	long long deviceId;
	unsigned int inodeId;
	double nbHardLinks;
	long long fullNbHardLinks;
	long long userId;
	long long groupId;
	long long size;
	long long realSize; /// size (takes hardlinks into account)
	long long sizeOnDisk; /// size on hard-drive (takes hardlinks into account)
	long long accessTime;
	long long modificationTime;
	long long creationTime;
};

}

#endif
