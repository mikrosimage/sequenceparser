#ifndef _SEQUENCE_PARSER_SEQUENCE_HPP_
#define _SEQUENCE_PARSER_SEQUENCE_HPP_

#include "FileObject.hpp"
#include "commonDefinitions.hpp"

#include <boost/lexical_cast.hpp>
#include <iomanip>
#include <set>

namespace sequenceParser {

namespace detail {
class FileNumbers;
}
class Detector;

/**
 * @brief A sequence of numbered files.
 */
class Sequence : public FileObject
{

public:
	friend class Detector;

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

	Sequence() :
	FileObject()
	{
		clear();
	}

	Sequence( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const EMaskOptions options = eMaskOptionsDefault, const bool strictPadding = false ) :
	FileObject( directory, eMaskTypeSequence, options )
	{
		init( prefix, padding, suffix, firstTime, lastTime, step, strictPadding );
	}

	Sequence( const boost::filesystem::path& directory, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault ) :
	FileObject( directory, eMaskTypeSequence, options )
	{
		init( pattern, firstTime, lastTime, step, accept );
	}

	Sequence( const boost::filesystem::path& directory, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault ) :
	FileObject( directory, eMaskTypeSequence, options )
	{
		init( firstTime, lastTime, step, accept );
	}

	/**
	 * @todo check if we put a pattern with full path: /home/foo/images/foo.####.jpg
	 */
	Sequence( const boost::filesystem::path& directory, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault );

	Sequence( const Sequence& v ) :
	FileObject( v._options )
	{
		operator=( v );
		_options = v._options;
	}

	Sequence( const boost::filesystem::path& directory, const Sequence& v, const EMaskOptions& options ) :
	FileObject( options )
	{
		operator=( v );
		_options = options;
		setDirectory( directory );
	}

	~Sequence();

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

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	inline bool init( const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );

public:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	inline bool initFromPattern( const boost::filesystem::path& dir, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault );

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

	/**
	 * @brief Find the biggest common step from a set of all steps.
	 */
	void extractStep( const std::set<std::size_t>& steps );

	/**
	 * @brief Extract step from a sorted list of time values.
	 */
	void extractStep( const std::list<Time>& times );

private:
	/**
	 * @brief Extract step from a sorted list of time values.
	 */
	void extractStep( const std::list<detail::FileNumbers>& times, const std::size_t i );

	std::size_t getPaddingFromStringNumber( const std::string& timeStr );

	/**
	 * @brief extract the padding from a list of frame numbers
	 * @param[in] timesStr list of frame numbers in string format
	 */
	void extractPadding( const std::list<std::string>& timesStr );

	void extractPadding( const std::list<detail::FileNumbers>& times, const std::size_t i );

	/**
	 * @brief return if the padding is strict (at least one frame begins with a '0' padding character).
	 * @param[in] timesStr list of frame numbers in string format
	 * @param[in] padding previously detected padding
	 */
	void extractIsStrictPadding( const std::list<std::string>& timesStr, const std::size_t padding );

	void extractIsStrictPadding( const std::list<detail::FileNumbers>& times, const std::size_t i, const std::size_t padding );

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

	inline std::size_t getStep() const;

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

	EPattern checkPattern( const std::string& pattern );

	bool operator<(const Sequence& other ) const
	{
		return getAbsoluteStandardPattern() < other.getAbsoluteStandardPattern();
	}

	bool operator==(const Sequence& other ) const
	{
		return
			( _prefix == other._prefix ) &&
			( _suffix == other._suffix ) &&
			( _strictPadding == other._strictPadding ) &&
			( _padding == other._padding ) &&
			( _step == other._step ) &&
			( _firstTime == other._firstTime ) &&
			( _lastTime == other._lastTime ) &&
			( _nbFiles == other._nbFiles );
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

private:

	std::ostream& getCout( std::ostream& os ) const;

	std::vector<boost::filesystem::path> getFiles() const;

protected:

	inline void clear()
	{
		FileObject::clear();
		_prefix.clear();
		_suffix.clear();
		_padding = 0;
		_step = 1;
		_firstTime = 0;
		_lastTime = 0;
		_nbFiles = 0;
	}

public:
	std::string _prefix; ///< filename prefix
	std::string _suffix; ///< filename suffix
	bool _strictPadding; ///<
	std::size_t _padding; ///< padding, no padding if 0, fixed padding otherwise
	std::size_t _step; ///< step
	Time _firstTime; ///< begin time
	Time _lastTime; ///< end time
	std::size_t _nbFiles; ///< number of frames
	static const char _fillCar = '0'; ///< Filling character
};

#include <Sequence.tcc>


}

#endif // _SEQUENCE_HPP_
