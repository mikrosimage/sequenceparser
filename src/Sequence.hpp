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
	
	Sequence* clone() const { return new Sequence(*this); }

private:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	void init( const std::string& prefix, const std::size_t padding, const size_t maxPadding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1 );

public:
	std::string getFilenameAt( const Time time ) const;

	inline std::string getFirstFilename() const;

	/// @return pattern character in standard style
	inline char getPatternCharacter() const;

	/// @return a string pattern using standard style
	inline std::string getStandardPattern() const;

	/// @return a string pattern using printf style
	inline std::string getPrintfPattern() const;

	/// @return a string pattern using C Style
	std::string getCStylePattern() const;

	inline std::pair<Time, Time> getGlobalRange() const;

	inline Time getFirstTime() const;

	inline Time getLastTime() const;

	inline std::size_t getDuration() const;

	Time getNbFiles() const;

	inline std::size_t getFixedPadding() const;

	inline std::size_t getMaxPadding() const;

	inline bool hasMissingFile() const;

	inline std::size_t getNbMissingFiles() const;

	/// @brief filename without frame number
	inline std::string getIdentification() const;

	inline std::string getPrefix() const;

	inline std::string getSuffix() const;

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[in] filename: filename to found
	 * @param[out] time: the time extracted from the filename (only if contained in the sequence)
	 * @param[out] timeStr: the time in string extracted from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& time, std::string& timeStr );

	EPattern checkPattern( const std::string& pattern, const EDetection detectionOptions );

	bool operator<(const Sequence& other ) const
	{
		return getStandardPattern() < other.getStandardPattern();
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
	std::vector<boost::filesystem::path> getFiles() const;

	/**
	 * @brief Iterate over files name contained in Sequence and concatenates file name to parentPath
	 * @param parentFolder
	 */
	std::vector<boost::filesystem::path> getAbsoluteFilesPath(boost::filesystem::path const& parentPath) const;

	std::vector<FrameRange>& getFrameRanges() { return _ranges; }
	const std::vector<FrameRange>& getFrameRanges() const { return _ranges; }

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
	std::string _prefix; ///< filename prefix
	std::string _suffix; ///< filename suffix
	/**
	 * @brief Number max of common padding used to enumerate the sequence
	 * @note For fixed sequences, it is equal to the padding
	 * @note Useful for sequence with a variable or an unknown padding
	 * unknown padding = when no frame begins with a '0' padding character
	 * seq.101.jpg
	 * seq.102.jpg
	 * seq.102.jpg
	 * variable padding = when not all frames have the same padding
	 * seq.0101.jpg
	 * seq.0100.jpg
	 * seq.099.jpg
	 */
	std::size_t _maxPadding;
	/**
	 * @brief Fixed padding
	 * @note if 0, variable padding
	 * @see _maxPadding
	 */
	std::size_t _fixedPadding;
	std::vector<FrameRange> _ranges;
	static const char _fillCar = '0'; ///< Filling character
};

std::ostream& operator<<(std::ostream& os, const Sequence& sequence);

#ifndef SWIG

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

#include <Sequence.tcc>


}

#endif
