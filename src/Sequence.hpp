#ifndef _SEQUENCE_PARSER_SEQUENCE_HPP_
#define _SEQUENCE_PARSER_SEQUENCE_HPP_

#include "FileObject.hpp"
#include "commonDefinitions.hpp"
#include "FrameRange.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <iomanip>
#include <set>


namespace sequenceParser {

namespace detail {
class FileNumbers;
}


/**
 * @brief A sequence of numbered files.
 */
class Sequence : public FileObject
{

public:
	/**
	 * List all recognized pattern types.
	 */
	enum EPattern
	{

		ePatternNone = 0,
		ePatternStandard = 1,
		ePatternCStyle = ePatternStandard * 2,
		ePatternFrame = ePatternCStyle * 2,
		ePatternFrameNeg = ePatternFrame * 2,

		ePatternDefault = ePatternCStyle + ePatternStandard,
		ePatternAll = ePatternCStyle + ePatternStandard + ePatternFrameNeg
	};

	Sequence()
	{
		clear();
	}

	Sequence( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const EDisplay displayOptions = eDisplayDefault, const bool strictPadding = false )
		: FileObject( directory, eTypeSequence, displayOptions )
	{
		init( prefix, padding, suffix, firstTime, lastTime, step, strictPadding );
	}

	Sequence( const boost::filesystem::path& directory, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EDisplay displayOptions = eDisplayDefault, const EPattern accept = ePatternDefault )
		: FileObject( directory, eTypeSequence, displayOptions )
	{
		init( pattern, firstTime, lastTime, step, accept );
	}

	Sequence( const boost::filesystem::path& filepath, const Time firstTime, const Time lastTime, const Time step, const EDisplay displayOptions = eDisplayDefault, const EPattern accept = ePatternDefault )
		: FileObject( filepath.parent_path(), eTypeSequence, displayOptions )
	{
		init( filepath.filename().string(), firstTime, lastTime, step, accept );
	}

	/**
	 * @todo check if we put a pattern with full path: /home/foo/images/foo.####.jpg
	 */
	Sequence( const boost::filesystem::path& directory, const EDisplay displayOptions = eDisplayDefault, const EPattern accept = ePatternDefault );

	Sequence( const Sequence& v )
	{
		operator=( v );
	}

	Sequence( const boost::filesystem::path& directory, const Sequence& v, const EDisplay& displayOptions )
	{
		operator=( v );
		_displayOptions = displayOptions;
		setDirectory( directory );
	}

	~Sequence();

	Sequence& operator=( const Sequence& other )
	{
		FileObject::operator=( other );
		_prefix = other._prefix;
		_suffix = other._suffix;
		_strictPadding = other._strictPadding;
		_padding = other._padding;
		_ranges = other._ranges;
		return *this;
	}
	
	Sequence* clone() const { return new Sequence(*this); }
	
private:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	void init( const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false );

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	bool init( const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );

public:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	inline bool initFromPattern( const boost::filesystem::path& dir, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );

	/**
	 * @brief Init from directory and pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	bool initFromDetection( const std::string& pattern, const EPattern accept = ePatternDefault );

	/**
	 * @brief Init from full pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	inline bool initFromDetection( const EPattern& accept = ePatternDefault );

public:
	inline std::string getAbsoluteFilenameAt( const Time time ) const;

	inline std::string getFilenameAt( const Time time ) const;

	inline std::string getFirstFilename() const;

	inline std::string getAbsoluteFirstFilename() const;

	inline std::string getAbsoluteLastFilename() const;

	/// @return pattern character in standard style
	inline char getPatternCharacter() const;

	/// @return a string pattern using standard style
	inline std::string getStandardPattern() const;

	inline std::string getAbsoluteStandardPattern() const;

	/// @return a string pattern using C Style
	inline std::string getCStylePattern() const;

	inline std::string getAbsoluteCStylePattern() const;

	inline std::pair<Time, Time> getRange() const;

	inline Time getFirstTime() const;

	inline Time getLastTime() const;

	inline std::size_t getDuration() const;

	inline Time getNbFiles() const;

	inline std::size_t getPadding() const;

	inline bool isStrictPadding() const;

	inline bool hasMissingFile() const;

	inline std::size_t getNbMissingFiles() const;

	/// @brief filename without frame number
	inline std::string getIdentification() const;

	inline std::string getPrefix() const;

	inline std::string getSuffix() const;

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[out] time: the time extract from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& time, std::string& timeStr );

	EPattern checkPattern( const std::string& pattern, const EDetection detectionOptions );

	bool operator<(const Sequence& other ) const
	{
		return getAbsoluteStandardPattern() < other.getAbsoluteStandardPattern();
	}

	bool operator==(const Sequence& other ) const
	{
		/*
		std::cout << "_prefix: " << _prefix << " -> " << other._prefix << std::endl;
		std::cout << "_suffix: " << _suffix << " -> " << other._suffix << std::endl;
		std::cout << "_padding: " << _padding << " -> " << other._padding << std::endl;
		std::cout << "_firstTime: " << _firstTime << " -> " << other._firstTime << std::endl;
		std::cout << "_lastTime: " << _lastTime << " -> " << other._lastTime << std::endl;
		std::cout << "_nbFiles: " << _nbFiles << " -> " << other._nbFiles << std::endl;
		*/
		return
			( _prefix == other._prefix ) &&
			( _suffix == other._suffix ) &&
			( _padding == other._padding ) &&
			( _ranges == other._ranges );
	}

	bool operator!=(const Sequence& other ) const
	{
		return !operator==( other );
	}

protected:

	/**
	 * @brief Partial initialization, using only pattern informations.
	 * @warning You don't have all informations like range, directory, etc.
	 */
	bool retrieveInfosFromPattern( const std::string& pattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding ) const;

public:

	std::ostream& getCout( std::ostream& os ) const;

	std::vector<boost::filesystem::path> getFiles() const;
	
	std::vector<FrameRange>& getFrameRanges() { return _ranges; }
	const std::vector<FrameRange>& getFrameRanges() const { return _ranges; }

	const FrameRangesView getFramesIterable() const
	{
		return FrameRangesView(getFrameRanges());
	}

protected:

	inline void clear()
	{
		FileObject::clear();
		_prefix.clear();
		_suffix.clear();
		_padding = 0;
		_ranges.clear();
	}

public:
	std::string _prefix; ///< filename prefix
	std::string _suffix; ///< filename suffix
	bool _strictPadding; ///<
	std::size_t _padding; ///< padding, no padding if 0, fixed padding otherwise
	std::vector<FrameRange> _ranges;
	static const char _fillCar = '0'; ///< Filling character
};

inline std::ostream& operator<<(std::ostream& os, const Sequence& sequence)
{
	return sequence.getCout(os);
}

#ifndef SWIG

std::vector<FrameRange> extractFrameRanges( const std::vector<Time>& times );

/**
 * @brief Extract step from a sorted vector of time values.
 */
std::size_t extractStep( const std::vector<Time>& times );

/**
 * @brief Extract step from a sorted vector of time values.
 */
std::size_t extractStep( const std::vector<detail::FileNumbers>::const_iterator& timesBegin, const std::vector<detail::FileNumbers>::const_iterator& timesEnd, const std::size_t i );

std::size_t getPaddingFromStringNumber( const std::string& timeStr );

/**
 * @brief extract the padding from a vector of frame numbers
 * @param[in] timesStr vector of frame numbers in string format
 */
std::size_t extractPadding( const std::vector<std::string>& timesStr );

std::size_t extractPadding( const std::vector<detail::FileNumbers>::const_iterator& timesBegin, const std::vector<detail::FileNumbers>::const_iterator& timesEnd, const std::size_t i );

/**
 * @brief return if the padding is strict (at least one frame begins with a '0' padding character).
 * @param[in] timesStr vector of frame numbers in string format
 * @param[in] padding previously detected padding
 */
bool extractIsStrictPadding( const std::vector<std::string>& timesStr, const std::size_t padding );

bool extractIsStrictPadding( const std::vector<detail::FileNumbers>& times, const std::size_t i, const std::size_t padding );

#endif

#include <Sequence.tcc>


}

#endif
