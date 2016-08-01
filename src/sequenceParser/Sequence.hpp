#ifndef _SEQUENCE_PARSER_SEQUENCE_HPP_
#define _SEQUENCE_PARSER_SEQUENCE_HPP_

#include "common.hpp"
#include "FrameRange.hpp"

#include <boost/lexical_cast.hpp>

#include <iomanip>
#include <set>


namespace sequenceParser {

namespace detail {
class FileNumbers;
}

/**
 * List all recognized pattern types.
 */
enum EPattern
{
	ePatternNone = 0,
	ePatternStandard = 1,
	ePatternCStyle = 2,
	ePatternFrame = 4,
	ePatternFrameNeg = 8,

	ePatternDefault = ePatternCStyle + ePatternStandard,
	ePatternAll = ePatternCStyle + ePatternStandard + ePatternFrameNeg
};


/**
 * @brief A sequence of numbered files. Has no parent folder notion.
 */
class Sequence
{
public:
	Sequence()
	{
		clear();
	}

	Sequence( const std::string& prefix, const std::size_t padding, const size_t maxPadding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1 )
	{
		init( prefix, padding, maxPadding, suffix, firstTime, lastTime, step );
	}

	Sequence( const std::string& pattern, const std::vector<FrameRange>& frameRanges, const EPattern accept = ePatternDefault )
	{
		if( initFromPattern( pattern, accept ) )
		{
			_ranges = frameRanges;
		}
	}

	Sequence( const Sequence& v )
	{
		operator=( v );
	}

#ifndef SWIG
	Sequence( const boost::filesystem::path& directory, const Sequence& v )
	{
		operator=( v );
	}

	Sequence& operator=( const Sequence& other )
	{
		_prefix = other._prefix;
		_suffix = other._suffix;
		_maxPadding = other._maxPadding;
		_fixedPadding = other._fixedPadding;
		_ranges = other._ranges;
		return *this;
	}
#endif

	Sequence* clone() const { return new Sequence(*this); }

private:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	void init( const std::string& prefix, const std::size_t padding, const size_t maxPadding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1 );

public:
	/// @return A list of filenames contained in the sequence.
	std::vector<std::string> getFiles() const;

	/**
	 * @return The path to the file at the given time.
	 * @warning There is no check to validate that the given time exists.
	 */
	std::string getFilenameAt( const Time time ) const;

	inline std::string getFirstFilename() const;

	inline std::string getLastFilename() const;

	/// @return pattern character in standard style
	inline char getPatternCharacter() const;

	/// @return a string pattern using standard style
	inline std::string getFilenameWithStandardPattern() const;

	/// @return a string pattern using printf style
	inline std::string getFilenameWithPrintfPattern() const;

	/// @return a string pattern using C Style
	std::string getCStylePattern() const;

	/**
	 * @return The frame range between first time and last time.
	 * @warning The range could have holes.
	 * @see hasMissingFile
	 */
	inline std::pair<Time, Time> getGlobalRange() const;

	inline Time getFirstTime() const;

	inline Time getLastTime() const;

	/**
	 * @return Times between first and last frame.
	 * @warning The range could have holes.
	 * @see getNbFiles
	 */
	inline std::size_t getDuration() const;

	Time getNbFiles() const;

	/**
	 * @return The size of the padding (if fixed padding).
	 * @note if 0, variable padding
	 * @see getMaxPadding
	 */
	inline std::size_t getFixedPadding() const;

	/**
	 * @return The number max of common padding used to enumerate the sequence
	 * @note For fixed sequences, it is equal to the padding
	 * @note Useful for sequence with a variable or an unknown padding
	 * unknown padding = when no frame begins with a '0' padding character
	 * seq.101.jpg
	 * seq.102.jpg
	 * seq.103.jpg
	 * variable padding = when not all frames have the same padding
	 * seq.0101.jpg
	 * seq.0100.jpg
	 * seq.099.jpg
	 */
	inline std::size_t getMaxPadding() const;

	inline bool hasMissingFile() const;

	inline std::size_t getNbMissingFiles() const;

	/// @return The filename without frame number (example: "sequence-.jpg" instead of "sequence-####.jpg")
	inline std::string getIdentification() const;

	/// @return The filename prefix (example: "sequence-" instead of "sequence-####.jpg")
	inline std::string getPrefix() const;

	/// @return The filename suffix (example: ".jpg" instead of "sequence-####.jpg")
	inline std::string getSuffix() const;

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[in] filename: filename to found
	 * @param[out] time: the time extracted from the filename (only if contained in the sequence)
	 * @param[out] timeStr: the time in string extracted from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& timeOut, std::string& timeStrOut );

	EPattern checkPattern( const std::string& pattern, const EDetection detectionOptions );

	bool operator<(const Sequence& other ) const
	{
		return getFilenameWithStandardPattern() < other.getFilenameWithStandardPattern();
	}

	bool operator==(const Sequence& other ) const
	{
		return
			( _prefix == other._prefix ) &&
			( _suffix == other._suffix ) &&
			( _maxPadding == other._maxPadding ) &&
			( _fixedPadding == other._fixedPadding ) &&
			( _ranges == other._ranges );
	}

	bool operator!=(const Sequence& other ) const
	{
		return !operator==( other );
	}

	/**
	 * @brief Partial initialization, using only pattern informations.
	 * @warning You don't have all informations like range, directory, etc.
	 */
	bool initFromPattern( const std::string& pattern, const EPattern& accept );

public:

#ifndef SWIG
	/**
	 * @brief Iterate over files name contained in Sequence and concatenates file name to parentPath
	 * @param parentFolder
	 */
	std::vector<boost::filesystem::path> getAbsoluteFilesPath(boost::filesystem::path const& parentPath) const;
#endif

	std::vector<FrameRange>& getFrameRanges() { return _ranges; }
#ifndef SWIG
	const std::vector<FrameRange>& getFrameRanges() const { return _ranges; }
#endif

	const FrameRangesView getFramesIterable() const
	{
		return FrameRangesView(getFrameRanges());
	}
	const FrameRangesSubView getFramesIterable( const Time first, const Time last ) const
	{
		return FrameRangesSubView( getFrameRanges(), first, last );
	}

	inline void clear()
	{
		_prefix.clear();
		_suffix.clear();
		_maxPadding = 0;
		_fixedPadding = 0;
		_ranges.clear();
	}
	
	std::string string() const;

public:
	std::string _prefix;
	std::string _suffix;

	std::size_t _maxPadding;
	std::size_t _fixedPadding;

	std::vector<FrameRange> _ranges;
	static const char _fillCar = '0'; ///< Filling character
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const Sequence& sequence);

/**
 * @brief Extract step from a sorted vector of time values.
 */
std::size_t extractStep( const std::vector<Time>& times );

/**
 * @brief Extract step from a sorted vector of time values.
 */
std::size_t extractStep( const std::vector<detail::FileNumbers>::const_iterator& timesBegin, const std::vector<detail::FileNumbers>::const_iterator& timesEnd, const std::size_t i );

std::size_t getFixedPaddingFromStringNumber( const std::string& timeStr );

/**
 * @brief extract the padding from a vector of frame numbers
 * @param[in] timesStr vector of frame numbers in string format
 */
std::size_t extractPadding( const std::vector<std::string>& timesStr );

std::size_t extractPadding( const std::vector<detail::FileNumbers>::const_iterator& timesBegin, const std::vector<detail::FileNumbers>::const_iterator& timesEnd, const std::size_t i );

#endif

#include "Sequence.tcc"


}

#endif
