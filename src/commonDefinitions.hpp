#ifndef _SEQUENCE_PARSER_COMMON_DEFINITIONS_HPP
#define _SEQUENCE_PARSER_COMMON_DEFINITIONS_HPP

#include <string>
#include <cstddef>

#ifdef _MSC_VER
#include <BaseTsd.h>
#else
#include <unistd.h>
#endif

// compatibility problems...
namespace std {
#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
#else
typedef ::ssize_t ssize_t;
#endif
}



namespace sequenceParser {

#ifndef PROPERTIES_WIDTH
#define PROPERTIES_WIDTH 3
#endif

#ifndef NAME_WIDTH
#define NAME_WIDTH 50
#endif

#ifndef NAME_WIDTH_WITH_DIR
#define NAME_WIDTH_WITH_DIR 80
#endif


#ifndef SEQUENCE_PARSER_OUTPUT_COLOR_FILES
#define SEQUENCE_PARSER_OUTPUT_COLOR_FILES
#ifdef __LINUX__
static const std::string kColorStd = "\E[0;0m";
static const std::string kColorFolder = "\E[1;34m";
static const std::string kColorFile = "\E[0;32m";
static const std::string kColorSequence = "\E[0;32m";
static const std::string kColorError = "\E[1;31m";
#else
static const std::string kColorStd = "";
static const std::string kColorFolder = "";
static const std::string kColorFile = "";
static const std::string kColorSequence = "";
static const std::string kColorError = "";
#endif
#endif

typedef std::ssize_t Time;

/**
 * List all recognized pattern types.
 */
enum EMaskType
{

	eMaskTypeUndefined = 0,
	eMaskTypeDirectory = 1,
	eMaskTypeFile = eMaskTypeDirectory * 2,
	eMaskTypeSequence = eMaskTypeFile * 2,
	eMaskTypeDefault = eMaskTypeSequence
};

enum EMaskOptions
{

	eMaskOptionsNone = 0, // 0
	eMaskOptionsProperties = 1, // show type of FileObject
	eMaskOptionsPath = eMaskOptionsProperties * 2, // show path of FileObject
	eMaskOptionsAbsolutePath = eMaskOptionsPath * 2, // show absolute path of FileObject
	eMaskOptionsRecursive = eMaskOptionsAbsolutePath * 2, // show recurssive listing
	eMaskOptionsNegativeIndexes = eMaskOptionsRecursive * 2, // show negative indexes sequences ( seq.-0020.jpg)
	eMaskOptionsSequenceBasedOnFilename = eMaskOptionsNegativeIndexes * 2, // list sequence based on a filename of the sequence
	eMaskOptionsDotFile = eMaskOptionsSequenceBasedOnFilename * 2, // show files which start with a dot (hidden files)
	eMaskOptionsColor = eMaskOptionsDotFile * 2, // output with color
	eMaskOptionsDefault = ( eMaskOptionsPath | eMaskOptionsColor )
};

inline EMaskType operator~(const EMaskType& a )
{
	return (EMaskType) ( ~int(a ) );
}

inline EMaskType operator&=( EMaskType& a, const EMaskType& b )
{
	return a = (EMaskType) (int(b ) & int(a ) );
}

inline EMaskType operator|=( EMaskType& a, const EMaskType& b )
{
	return a = (EMaskType) (int(b ) | int(a ) );
}

inline EMaskType operator|( const EMaskType& a, const EMaskType& b )
{
	return (EMaskType) (int(b ) | int(a ) );
}

inline EMaskOptions operator|=( EMaskOptions& a, const EMaskOptions& b )
{
	a = (EMaskOptions) (int(b ) | int(a ) );
	return a;
}

inline EMaskOptions operator|( const EMaskOptions& a, const EMaskOptions& b )
{
	return (EMaskOptions) (int(b ) | int(a ) );
}

inline EMaskOptions remove( EMaskOptions& a, const EMaskOptions& b )
{
	a = (EMaskOptions) (int(~b ) & int(a ) );
	return a;
}

template<typename T>
inline T greatestCommonDivisor( T a, T b )
{
	T r;
	if( b == 0 )
		return 0;
	while( ( r = a % b ) != 0 )
	{
		a = b;
		b = r;
	}
	return b;
}

}


#endif
