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


#ifndef SWIG
#ifndef SEQUENCE_PARSER_OUTPUT_COLOR_FILES
#define SEQUENCE_PARSER_OUTPUT_COLOR_FILES
#if (defined __UNIX__ || defined UNIX || defined __LINUX__ || defined LINUX )
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
#endif

typedef ::std::ssize_t Time;

#define SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(ENUM_TYPE) \
\
inline ENUM_TYPE operator~(const ENUM_TYPE& a ) \
{ \
	return (ENUM_TYPE)(~int(a)); \
} \
\
inline ENUM_TYPE operator&=( ENUM_TYPE& a, const ENUM_TYPE& b ) \
{ \
	return a = (ENUM_TYPE)(int(b) & int(a)); \
} \
\
inline ENUM_TYPE operator&( const ENUM_TYPE& a, const ENUM_TYPE& b ) \
{ \
	return (ENUM_TYPE)(int(b) & int(a)); \
} \
\
inline ENUM_TYPE operator|=( ENUM_TYPE& a, const ENUM_TYPE& b ) \
{ \
	return a = (ENUM_TYPE)(int(b) | int(a) ); \
} \
\
inline ENUM_TYPE operator|( const ENUM_TYPE& a, const ENUM_TYPE& b ) \
{ \
	return (ENUM_TYPE)(int(b) | int(a)); \
}

enum EType
{
	eTypeUndefined = 0,
	eTypeFolder = 1,
	eTypeFile = 2,
	eTypeSequence = 4
};

enum EDetection
{
	eDetectionNone = 0,
	/// detect negative numbers (instead of detecting "-" as a non-digit character)
	eDetectionNegative = 1,
	/// A file alone with a number in the filename could be considered as a simple
	/// file or a sequence of 1 item.
	eDetectionSequenceNeedAtLeastTwoFiles = 2,
	/// By default, single file sequence use the latest number of the filename.
	/// You could use this option to use the first number instead.
	eDetectionSingleFileSeqUseFirstNumber = 4,
	eDetectionIgnoreDotFile = 8,
	/// detect from an existing filename of the sequence
	eDetectionSequenceFromFilename = 16,
	eDetectionDefault = (eDetectionSequenceNeedAtLeastTwoFiles | eDetectionIgnoreDotFile | eDetectionSequenceFromFilename)
};

/// TODO: remove that!
enum EDisplay
{
	eDisplayNone = 0,
	eDisplayProperties = 1,
	eDisplayPath = 2,
	eDisplayAbsolutePath = 4,
	eDisplayColor = 8,
	eDisplayDefault = (eDisplayPath | eDisplayColor)
};

SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(EType)
SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(EDetection)
SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(EDisplay)

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
