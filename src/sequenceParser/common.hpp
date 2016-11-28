#ifndef _SEQUENCE_PARSER_COMMON_DEFINITIONS_HPP
#define _SEQUENCE_PARSER_COMMON_DEFINITIONS_HPP

#define SEQUENCEPARSER_VERSION_MAJOR 2
#define SEQUENCEPARSER_VERSION_MINOR 1
#define SEQUENCEPARSER_VERSION_MICRO 0

#include <string>
#include <cstddef>

#ifdef _MSC_VER
#include <BaseTsd.h>
#else
#include <unistd.h>
#endif

// Compatibility problems
namespace std {
#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
#else
typedef ::ssize_t ssize_t;
#endif
}

// Forward declaration
namespace boost {
namespace filesystem {
class path;
}
}

#ifdef SWIGJAVA
#include <boost/locale.hpp>
#endif

namespace sequenceParser {

#ifndef SWIG
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
#endif

enum EType
{
	eTypeUndefined = 0,
	eTypeFolder = 1,
	eTypeFile = 2,
	eTypeSequence = 4,
	eTypeLink = 8,
	eTypeAll = (eTypeFolder | eTypeFile | eTypeSequence | eTypeLink)
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
	/// detect sequences without holes
	eDetectionSequenceWithoutHoles = 32,
	eDetectionDefaultWithDotFile = (eDetectionSequenceNeedAtLeastTwoFiles | eDetectionSequenceFromFilename),
	eDetectionDefault = (eDetectionSequenceNeedAtLeastTwoFiles | eDetectionIgnoreDotFile | eDetectionSequenceFromFilename)
};

#ifndef SWIG
SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(EType)
SEQUENCEPARSER_ENUM_BITWISE_OPERATORS(EDetection)
#endif

#ifdef SWIGJAVA
/**
 * @brief Strings are retrieved from JNI using GetStringUTFChars.
 * So there is an implicit UTF8 conversion.
 */
std::string utf8_to_latin1( const std::string& utf8_path )
{
	using namespace boost::locale::conv;
	std::string latin1_path = from_utf<char>(utf8_path, "Latin1");
	return latin1_path;
}
#endif

}


#endif
