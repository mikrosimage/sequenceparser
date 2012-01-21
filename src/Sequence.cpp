#include "Sequence.hpp"
#include "Folder.hpp"
#include "File.hpp"

#include "detail/FileNumbers.hpp"

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <set>

namespace sequenceParser {

namespace bfs = boost::filesystem;

/// All regex to recognize a pattern
// common used pattern with # or @
static const boost::regex regexPatternStandard("(.*?)" // anything but without priority
						   "\\[?" // if pattern is myimage[####].jpg, don't capture []
						   "(#+|@+)" // we capture all # or @
						   "\\]?" // possible end of []
						   "(.*?)" // anything
		);
// C style pattern
static const boost::regex regexPatternCStyle("(.*?)" // anything but without priority
						 "\\[?" // if pattern is myimage[%04d].jpg, don't capture []
						 "%([0-9]*)d" // we capture the padding value (eg. myimage%04d.jpg)
						 "\\]?" // possible end of []
						 "(.*?)" // anything
		);
// image name
static const boost::regex regexPatternFrame("(.*?" // anything but without priority
						"[_\\.]?)" // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
						"\\[?" // if pattern is myimage[0001].jpg, don't capture []
						"([\\-\\+]?[0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
						"\\]?" // possible end of []
						"([_\\.]?" // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
						".*\\.?" //
						".*?)" // anything
		);

Sequence::~Sequence()
{
}

Sequence::Sequence(const boost::filesystem::path& directory, const EMaskOptions options, const EPattern accept) :
    FileObject(directory, eMaskTypeSequence, options)
{
    clear();
    initFromDetection(accept);
}

bool Sequence::isIn(const std::string& filename, Time& time, std::string& timeStr)
{
    std::size_t min = _prefix.size() + _suffix.size();

    if (filename.size() <= min)
        return false;

    if (filename.substr(0, _prefix.size()) != _prefix || filename.substr(filename.size() - _suffix.size(), _suffix.size()) != _suffix)
        return false;

    try
    {
        timeStr = filename.substr(_prefix.size(), filename.size() - _suffix.size() - _prefix.size());
        time = boost::lexical_cast<Time>(timeStr);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

Sequence::EPattern Sequence::checkPattern(const std::string& pattern)
{
    if ( regex_match( pattern.c_str(), regexPatternStandard ) )
    {
        return ePatternStandard;
    }
    else if ( regex_match( pattern.c_str(), regexPatternCStyle ) )
    {
        return ePatternCStyle;
    }
    else if ( regex_match( pattern.c_str(), regexPatternFrame ) )
    {
        return ePatternFrame;
    }
    return ePatternNone;
}

/**
 * @brief This function creates a regex from the pattern,
 *        and init internal values.
 * @param[in] pattern
 * @param[in] accept
 * @param[out] prefix
 * @param[out] suffix
 * @param[out] padding
 * @param[out] strictPadding
 */
bool Sequence::retrieveInfosFromPattern(const std::string& filePattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding) const {
    boost::cmatch matches;
    if ((accept & ePatternStandard) && regex_match(filePattern.c_str(), matches, regexPatternStandard))
    {
        std::string paddingStr(matches[2].first, matches[2].second);
	padding       = paddingStr.size();
        strictPadding = (paddingStr[0] == '#');
    }
    else if ((accept & ePatternCStyle) && regex_match(filePattern.c_str(), matches, regexPatternCStyle))
    {
        std::string paddingStr(matches[2].first, matches[2].second);
	padding       = paddingStr.size() == 0 ? 0 : boost::lexical_cast<std::size_t>(paddingStr); // if no padding value: %d -> padding = 0
        strictPadding = false;
    }
    else if ((accept & ePatternFrame) && regex_match(filePattern.c_str(), matches, regexPatternFrame))
    {
        std::string frame(matches[2].first, matches[2].second);
	// Time t = boost::lexical_cast<Time>( frame );
	padding       = frame.size();
        strictPadding = false;
    }
    else
    {
        // this is a file, not a sequence
        return false;
    }
    prefix = std::string(matches[1].first, matches[1].second);
    suffix = std::string(matches[3].first, matches[3].second);
    return true;
}

void Sequence::init( const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step, const bool strictPadding )
{
    _prefix        = prefix;
    _padding       = padding;
    _suffix        = suffix;
    _firstTime     = firstTime;
    _lastTime      = lastTime;
    _step          = step;
    _strictPadding = strictPadding;
    _nbFiles       = 0;
}

bool Sequence::init( const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept )
{
    if ( !retrieveInfosFromPattern( pattern, accept, _prefix, _suffix, _padding, _strictPadding ) )
        return false; // not regognize as a pattern, maybe a still file
    _firstTime = firstTime;
    _lastTime  = lastTime;
    _step      = step;
    _nbFiles   = 0;
    return true;
}

bool Sequence::initFromDetection( const std::string& pattern, const EPattern accept )
{
    clear();
    setDirectoryFromPath(pattern);

    if ( !retrieveInfosFromPattern( boost::filesystem::path(pattern).filename().string(), accept, _prefix, _suffix, _padding, _strictPadding ) )
        return false; // not recognized as a pattern, maybe a still file
    if ( !boost::filesystem::exists( _directory ) )
        return true; // an empty sequence

    std::list<std::string>  allTimesStr;
    std::list<Time>         allTimes;
    bfs::directory_iterator itEnd;

    for ( bfs::directory_iterator iter(_directory); iter != itEnd; ++iter )
    {
	// we don't make this check, which can take long time on big sequences (>1000 files)
	// depending on your filesystem, we may need to do a stat() for each file
	//      if( bfs::is_directory( iter->status() ) )
	//          continue; // skip directories
	Time        time;
	std::string timeStr;

	// if the file is inside the sequence
	if ( isIn(iter->path().filename().string(), time, timeStr ) )
	{
	    // create a big list of all times in our sequence
	    allTimesStr.push_back(timeStr);
	    allTimes.push_back(time);
        }
    }
    if (allTimes.size() < 2)
    {
	if (allTimes.size() == 1)
	{
	    _firstTime = _lastTime = allTimes.front();
	}
	return true; // an empty sequence
    }
    allTimes.sort();
    extractStep            ( allTimes );
    extractPadding         ( allTimesStr );
    extractIsStrictPadding ( allTimesStr, _padding );
    _firstTime     = allTimes.front         ( );
    _lastTime      = allTimes.back          ( );
    _nbFiles       = allTimes.size          ( );
    return true; // a real file sequence
}

/**
 * @brief Find the biggest common step from a set of all steps.
 */
void Sequence::extractStep(const std::set<std::size_t>& steps)
{
    if ( steps.size() == 1 )
    {
	_step = *steps.begin();
	return;
    }
    std::set<std::size_t> allSteps;
    for ( std::set<std::size_t>::const_iterator itA = steps.begin(), itB = ++steps.begin(), itEnd = steps.end(); itB != itEnd; ++itA, ++itB )
    {
	allSteps.insert( greatestCommonDivisor( *itB, *itA ) );
    }
    extractStep( allSteps );
}

/**
 * @brief Extract step from a sorted list of time values.
 */
void Sequence::extractStep( const std::list<Time>& times )
{
    if ( times.size() <= 1 )
    {
	_step = 1;
	return;
    }
    std::set<std::size_t> allSteps;
    for ( std::list<Time>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end(); itB != itEnd; ++itA, ++itB )
    {
	allSteps.insert( *itB - *itA );
    }
   extractStep( allSteps );
}

/**
 * @brief Extract step from a sorted list of time values.
 */
void Sequence::extractStep( const std::list<detail::FileNumbers>& times, const std::size_t i )
{
    if ( times.size() <= 1 )
    {
	_step = 1;
	return;
    }
    std::set<std::size_t> allSteps;
    for ( std::list<detail::FileNumbers>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end(); itB != itEnd; ++itA, ++itB )
    {
	allSteps.insert( itB->getTime(i) - itA->getTime(i) );
    }
    extractStep( allSteps );
}

std::size_t Sequence::getPaddingFromStringNumber( const std::string& timeStr )
{
    if (timeStr.size() > 1)
    {
	// if the number is signed, this charater does not count as padding.
	if (timeStr[0] == '-' || timeStr[0] == '+')
	{
	    return timeStr.size() - 1;
	}
    }
    return timeStr.size();
}

/**
 * @brief extract the padding from a list of frame numbers
 * @param[in] timesStr list of frame numbers in string format
 */
void Sequence::extractPadding( const std::list<std::string>& timesStr )
{
    BOOST_ASSERT( timesStr.size() > 0 );
    const std::size_t padding = getPaddingFromStringNumber(timesStr.front());

    BOOST_FOREACH( const std::string& s, timesStr )
    {
	if (padding != getPaddingFromStringNumber(s))
	{
	    _padding = 0;
	    return;
	}
    }
    _padding = padding;
}

void Sequence::extractPadding( const std::list<detail::FileNumbers>& times, const std::size_t i )
{
    BOOST_ASSERT( times.size() > 0 );
    const std::size_t padding = times.front().getString(i).size();

    BOOST_FOREACH( const detail::FileNumbers& s, times )
    {
	if ( padding != s.getString(i).size() )
	{
	    _padding = 0;
	    return;
	}
    }
    _padding = padding;
}

/**
 * @brief return if the padding is strict (at least one frame begins with a '0' padding character).
 * @param[in] timesStr list of frame numbers in string format
 * @param[in] padding previously detected padding
 */
void Sequence::extractIsStrictPadding( const std::list<std::string>& timesStr, const std::size_t padding )
{
    if ( padding == 0 )
    {
	_strictPadding = false;
	return;
    }

    BOOST_FOREACH( const std::string& s, timesStr )
    {
	if ( s[0] == '0' )
	{
	    _strictPadding = true;
	    return;
	}
    }
    _strictPadding = false;
}

void Sequence::extractIsStrictPadding( const std::list<detail::FileNumbers>& times, const std::size_t i, const std::size_t padding )
{
    if (padding == 0)
    {
	_strictPadding = false;
	return;
    }

    BOOST_FOREACH( const detail::FileNumbers& s, times )
    {
	if ( s.getString(i)[0] == '0' )
	{
	    _strictPadding = true;
	    return;
	}
    }
    _strictPadding = false;
}

std::ostream& Sequence::getCout(std::ostream& os) const
{
    bfs::path dir;
    if ( showAbsolutePath() )
    {
	dir = bfs::absolute( _directory );
        dir = boost::regex_replace(dir.string(), boost::regex("/\\./"), "/");
    }
    os << std::left;
    if ( showProperties() )
    {
	os << std::setw( PROPERTIES_WIDTH ) << "s ";
    }
    if ( showRelativePath() )
    {
        dir = _directory;
	dir = boost::regex_replace( dir.string(), boost::regex( "/\\./" ), "/" );
	os << std::setw( NAME_WIDTH_WITH_DIR ) << _kColorSequence + ( dir / getStandardPattern() ).string() + _kColorStd;
    }
    else
    {
	os << std::setw( NAME_WIDTH ) << _kColorSequence + ( dir / getStandardPattern() ).string() + _kColorStd;
    }

    os << " [" << getFirstTime() << ":" << getLastTime();
    if (getStep() != 1)
        os << "x" << getStep();
    os << "] " << getNbFiles() << " file" << ( ( getNbFiles() > 1 ) ? "s" : "");
    if (hasMissingFile())
    {
	os << ", " << _kColorError << getNbMissingFiles() << " missing file" << ( ( getNbMissingFiles() > 1 ) ? "s" : "" ) << _kColorStd;
    }
    return os;
}

std::vector<boost::filesystem::path> Sequence::getFiles() const
{
    std::vector<boost::filesystem::path> allPaths;
    for (Time t = getFirstTime(); t <= getLastTime(); t += getStep())
    {
        allPaths.push_back(getAbsoluteFilenameAt(t));
    }
    return allPaths;
}

}
