#include <Detector.hpp>
#include <Sequence.hpp>
#include <Folder.hpp>
#include <File.hpp>
#include <FileStrings.hpp>
#include <FileNumbers.hpp>
#include <commonDefinitions.hpp>

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <set>

namespace bfs = boost::filesystem;

namespace sequenceParser {

// NOTE How we can replace this with a wrapper?
// Like boost::function, boost::bind,...
struct SeqIdHash : std::unary_function<FileStrings, std::size_t>
{
    std::size_t operator()( const FileStrings& p ) const
    {
	return p.getHash();
    }
};


Detector::Detector()
{
}

Detector::~Detector()
{
}

std::list<boost::shared_ptr<File> > Detector::fileInDirectory( const bfs::path& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
    std::list<boost::shared_ptr<File> >  outputFiles;
    bfs::path directory;

    if ( exists( dir ) )
    {
	directory = bfs::path( dir.string() );
    }
    else
    {
	if( ! exists( dir.parent_path() ) )
	{
		return outputFiles;
	}
	filters.push_back( dir.filename().string() );
	directory = bfs::path( dir.parent_path().string() );
    }

    // variables for sequence detection
    typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
    SeqIdMap                                                                     sequences;
    FileStrings                                                                  id;        // an object uniquely identify a sequence
    FileNumbers                                                                  nums;      // the list of numbers inside one filename

    // for all files in the directory
    bfs::directory_iterator itEnd;
    for (bfs::directory_iterator iter(directory); iter != itEnd; ++iter)
    {
	// clear previous infos
	id.clear();
	nums.clear(); // (clear but don't realloc the vector inside)

	if ( !(iter->path().filename().string()[0] == '.') || (desc & eMaskOptionsDotFile) ) // if we ask to show hidden files and if it is hidden
	{

	    if ( ! bfs::is_directory( iter->status() ) ) // detect if isn't a folder
	    {
		// it's a file or a file of a sequence
		if ( isNotFilter( iter->path().filename().string(), filters ) ) // filtering of entries with filters strings
		{
		    // if at least one number detected
		    if ( seqConstruct(iter->path().filename().string(), id, nums ) )
		    {
			// need to construct sequence to detect file with a pattern but with only one image
			const SeqIdMap::iterator it( sequences.find( id ) );
			if (it != sequences.end()) // is already in map
			{
			    // append the list of numbers
			    sequences.at(id).push_back(nums);
			}
			else
			{
			    // create an entry in the map
			    std::list<FileNumbers> li;
			    li.push_back(nums);
			    sequences.insert(SeqIdMap::value_type(id, li));
			}
		    }
		    else
		    {
			boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
			outputFiles.push_back(f);
		    }
		}
	    }
	}
    }
    // add sequences in the output list
    BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
    {
	const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );
	BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
	{
	    // don't detect sequence of directories
	    if ( ! bfs::is_directory( s.getAbsoluteFirstFilename() ) )
	    {
		if ( s.getNbFiles() == 1 ) // if it's a sequence of 1 file, it isn't a sequence but only a file
		{
		    boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), s.getMaskOptions() ) );
		    outputFiles.push_back(file);
		}
	   }
       }
    }

    return outputFiles;
}

std::list<boost::shared_ptr<Sequence> > Detector::sequenceInDirectory( const bfs::path& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
    std::list<boost::shared_ptr<Sequence> >  outputSequences;
    bfs::path directory;

    if ( exists( dir ) )
    {
	directory = bfs::path( dir.string() );
    }
    else
    {
	if( ! exists( dir.parent_path() ) )
	{
		return outputSequences;
	}
	filters.push_back( dir.filename().string() );
	directory = bfs::path( dir.parent_path().string() );
    }

    // variables for sequence detection
    typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
    SeqIdMap                                                                     sequences;
    FileStrings                                                                  id;        // an object uniquely identify a sequence
    FileNumbers                                                                  nums;      // the list of numbers inside one filename

    // for all files in the directory
    bfs::directory_iterator itEnd;
    for (bfs::directory_iterator iter(directory); iter != itEnd; ++iter)
    {
	// clear previous infos
	id.clear();
	nums.clear(); // (clear but don't realloc the vector inside)

	if (!(iter->path().filename().string()[0] == '.') || (desc & eMaskOptionsDotFile)) // if we ask to show hidden files and if it is hidden
	{
	    // detect if isn't a folder
	    if ( ! bfs::is_directory( iter->status() ) )
	    { // it's a file or a file of a sequence
		if ( isNotFilter( iter->path().filename().string(), filters ) ) // filtering of entries with filters strings
		{
		    // if at least one number detected
		    if (seqConstruct(iter->path().filename().string(), id, nums))
		    {
			const SeqIdMap::iterator it(sequences.find(id));
			if (it != sequences.end()) // is already in map
			{
			    // append the list of numbers
			    sequences.at(id).push_back(nums);
			}
			else
			{
			    // create an entry in the map
			    std::list<FileNumbers> li;
			    li.push_back(nums);
			    sequences.insert(SeqIdMap::value_type(id, li));
			}
		    }
		}
	    }
	}
    }
    // add sequences in the output list
    BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
    {
	const std::list<Sequence> ss = buildSequence(directory, p.first, p.second, desc);
	BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
	{
	    // don't detect sequence of directories
	    if (!bfs::is_directory(s.getAbsoluteFirstFilename()))
	    {
		if ( ! ( s.getNbFiles() == 1 ) ) // if it's a sequence of 1 file, it isn't a sequence but only a file
		{
		    boost::shared_ptr<Sequence> seq(new Sequence(directory, s, desc));
		    outputSequences.push_back(seq);
		}
	   }
       }
    }

    return outputSequences;
}

std::list<boost::shared_ptr<FileObject> > Detector::fileAndSequenceInDirectory( const bfs::path& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
    std::list<boost::shared_ptr<FileObject> >  output;
    std::list<boost::shared_ptr<FileObject> >  outputFiles;
    std::list<boost::shared_ptr<FileObject> >  outputSequences;
    bfs::path directory;

    if ( exists( dir ) )
    {
	directory = bfs::path( dir.string() );
    }
    else
    {
	if( ! exists( dir.parent_path() ) )
	{
		return output;
	}
	filters.push_back( dir.filename().string() );
	directory = bfs::path( dir.parent_path().string() );
    }

    // variables for sequence detection
    typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
    SeqIdMap                                                                     sequences;
    FileStrings                                                                  id;        // an object uniquely identify a sequence
    FileNumbers                                                                  nums;      // the list of numbers inside one filename

    // for all files in the directory
    bfs::directory_iterator itEnd;
    for (bfs::directory_iterator iter(directory); iter != itEnd; ++iter)
    {
	// clear previous infos
	id.clear();
	nums.clear(); // (clear but don't realloc the vector inside)

	if (!(iter->path().filename().string()[0] == '.') || (desc & eMaskOptionsDotFile)) // if we ask to show hidden files and if it is hidden
	{
	    // detect if isn't a folder
	    if ( ! bfs::is_directory( iter->status() ) )
	    { // it's a file or a file of a sequence

		if ( isNotFilter( iter->path().filename().string(), filters ) ) // filtering of entries with filters strings
		{
		    // if at least one number detected
		    if (seqConstruct(iter->path().filename().string(), id, nums))
		    {
			const SeqIdMap::iterator it(sequences.find(id));
			if (it != sequences.end()) // is already in map
			{
			    // append the list of numbers
			    sequences.at(id).push_back(nums);
			}
			else
			{
			    // create an entry in the map
			    std::list<FileNumbers> li;
			    li.push_back(nums);
			    sequences.insert(SeqIdMap::value_type(id, li));
			}
		    }
		    else
		    {
			boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
			outputFiles.push_back(f);
		    }
		}
	    }
	}
    }
    // add sequences in the output list
    BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
    {
	const std::list<Sequence> ss = buildSequence(directory, p.first, p.second, desc);
	BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
	{
	    // don't detect sequence of directories
	    if (!bfs::is_directory(s.getAbsoluteFirstFilename()))
	    {
		if (s.getNbFiles() == 1) // if it's a sequence of 1 file, it isn't a sequence but only a file
		{
		    boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), s.getMaskOptions() ) );
		    outputFiles.push_back(file);
		}
		else
		{
		    boost::shared_ptr<Sequence> seq( new Sequence( directory, s, desc ) );
		    outputSequences.push_back(seq);
		}
	   }
       }
    }

    output.merge(outputFiles);
    output.merge(outputSequences);
    return output;
}

std::list<boost::shared_ptr<Folder> > Detector::folderInDirectory( const bfs::path& dir, std::vector<std::string>& filters, const EMaskOptions desc )
{
    std::list<boost::shared_ptr<Folder> >  outputFolders;
    bfs::path directory;

    if ( exists( dir ) )
    {
	directory = bfs::path( dir.string() );
    }
    else
    {
	if( ! exists( dir.parent_path() ) )
	{
		return outputFolders;
	}
	filters.push_back( dir.filename().string() );
	directory = bfs::path( dir.parent_path().string() );
    }

    // for all files in the directory
    bfs::directory_iterator itEnd;
    for ( bfs::directory_iterator iter( directory ); iter != itEnd; ++iter )
    {
	if (!(iter->path().filename().string()[0] == '.') || (desc & eMaskOptionsDotFile)) // if we ask to show hidden files and if it is hidden
	{
	    // detect if is a folder
	    if ( bfs::is_directory( iter->status() ) )
	    {
		boost::shared_ptr<Folder> d(new Folder(directory, iter->path().filename().string(), desc));
		outputFolders.push_back(d);
	    }
	}
    }
    return outputFolders;
}

std::list<boost::shared_ptr<FileObject> > Detector::fileObjectInDirectory( const bfs::path& dir, std::vector<std::string>& filters, const EMaskType mask, const EMaskOptions desc )
{
    std::list<boost::shared_ptr<FileObject> >  output;
    std::list<boost::shared_ptr<FileObject> >  outputFolders;
    std::list<boost::shared_ptr<FileObject> >  outputFiles;
    std::list<boost::shared_ptr<FileObject> >  outputSequences;
    bfs::path directory;

    if ( exists( dir ) )
    {
	directory = bfs::path( dir.string() );
    }
    else
    {
	if( ! exists( dir.parent_path() ) )
	{
		return output;
	}
	filters.push_back( dir.filename().string() );
	directory = bfs::path( dir.parent_path().string() );
    }

    // variables for sequence detection
    typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
    SeqIdMap                                                                     sequences;
    FileStrings                                                                  id;        // an object uniquely identify a sequence
    FileNumbers                                                                  nums;      // the list of numbers inside one filename

    // for all files in the directory
    bfs::directory_iterator itEnd;
    for (bfs::directory_iterator iter(directory); iter != itEnd; ++iter)
    {
	// clear previous infos
	id.clear();
	nums.clear(); // (clear but don't realloc the vector inside)

	if (!(iter->path().filename().string()[0] == '.') || (desc & eMaskOptionsDotFile)) // if we ask to show hidden files and if it is hidden
	{
	    // detect if is a folder
	    if ( bfs::is_directory( iter->status() ) )
	    {
		boost::shared_ptr<Folder> d(new Folder(directory, iter->path().filename().string(), desc));
		outputFolders.push_back(d);
	    }
	    else // it's a file or a file of a sequence
	    {
		if ( isNotFilter( iter->path().filename().string(), filters ) ) // filtering of entries with filters strings
		{
		    // if at least one number detected
		    if (seqConstruct(iter->path().filename().string(), id, nums))
		    {
			const SeqIdMap::iterator it(sequences.find(id));
			if (it != sequences.end()) // is already in map
			{
			    // append the list of numbers
			    sequences.at(id).push_back(nums);
			}
			else
			{
			    // create an entry in the map
			    std::list<FileNumbers> li;
			    li.push_back(nums);
			    sequences.insert(SeqIdMap::value_type(id, li));
			}
		    }
		    else
		    {
			boost::shared_ptr<File> f( new File( directory, iter->path().filename().string(), desc ) );
			outputFiles.push_back(f);
		    }
		}
	    }
	}
    }
    // add sequences in the output list
    BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
    {
	const std::list<Sequence> ss = buildSequence(directory, p.first, p.second, desc);
	BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
	{
	    // don't detect sequence of directories
	    if (!bfs::is_directory(s.getAbsoluteFirstFilename()))
	    {
		if (s.getNbFiles() == 1) // if it's a sequence of 1 file, it isn't a sequence but only a file
		{
		    boost::shared_ptr<File> file( new File( directory, s.getFirstFilename(), s.getMaskOptions() ) );
		    outputFiles.push_back( file );
		}
		else
		{
		    boost::shared_ptr<Sequence> seq( new Sequence(directory, s, desc ) );
		    outputSequences.push_back( seq );
		}
	   }
       }
    }

    if (mask & eMaskTypeDirectory)
    {
	output.merge(outputFolders);
    }
    // add files in the output list
    if (mask & eMaskTypeFile)
    {
	output.merge(outputFiles);
    }
    // add sequences in the output list
    if (mask & eMaskTypeSequence)
    {
	output.merge(outputSequences);
    }
    return output;
}

bool Detector::isNotFilter(std::string filename, std::vector<std::string>& filters)
{
    if (filters.size() == 0)
	return true;

    for (std::size_t i = 0; i < filters.size(); i++)
    {
	std::string filter(filters.at(i));
	filter = boost::regex_replace(filter, boost::regex("\\*"), "(.*)");
	filter = boost::regex_replace(filter, boost::regex("\\@"), "(.*)");
	filter = boost::regex_replace(filter, boost::regex("\\?"), "(.)");
	filter = boost::regex_replace(filter, boost::regex("\\#"), "(.)");
	if (regex_match(filename, boost::regex(filter)))
	    return true;
    }
    return false;
}

/**
 * @brief Create a sequence
 * @param[in] id: the sequence identifier
 * @param[inout] nums: the list of sequence of number inside each filename
 * @warning We modify nums in place, because we don't use it after
 *          so there is no reason to create a copy.
 * @return a sequence object with all informations
 */
std::list<Sequence> Detector::buildSequence( const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums, const EMaskOptions& desc )
{
    nums.sort();
    BOOST_ASSERT( nums.size() > 0 );
    // assert all FileNumbers have the same size...
    BOOST_ASSERT( nums.front().size() == nums.back().size() );
    std::size_t len = nums.front().size();

    // detect which part is the sequence number
    // for the moment, accept only one sequence
    // but we can easily support multi-sequences
    std::vector<std::size_t> allIds; // list of ids (with 0<id<len) with value changes
    for (std::size_t i = 0; i < len; ++i)
    {
	const Time t = nums.front().getTime(i);
	BOOST_FOREACH( const FileNumbers& sn, nums )
	{
	    if (sn.getTime(i) != t) {
		allIds.push_back( i );
		break;
	    }
	}
    }
    std::size_t idChangeBegin = 0;
    std::size_t idChangeEnd   = 0;
    if ( allIds.size() == 0 )
    {
	idChangeBegin = idChangeEnd = len - 1;
    }
    else
    {
	idChangeBegin = allIds.front();
	idChangeEnd   = allIds.back();
    }
    Sequence seqCommon(directory, desc);
    // fill information in the sequence...
    for (std::size_t i = 0; i < idChangeBegin; ++i) {
	seqCommon._prefix += id[i];
	seqCommon._prefix += nums.front().getString(i);
    }
    seqCommon._prefix += id[idChangeBegin];
    for (std::size_t i = idChangeEnd + 1; i < len; ++i) {
	seqCommon._suffix += id[i];
	seqCommon._suffix += nums.front().getString(i);
    }
    seqCommon._suffix += id[len];
    std::list<Sequence> result;
    if (allIds.size() <= 1) {
	// standard case, one sequence detected
	seqCommon._firstTime = nums.front().getTime( idChangeEnd );
	seqCommon._lastTime  = nums.back().getTime( idChangeEnd );
	seqCommon._nbFiles   = nums.size();

	seqCommon.extractStep            ( nums, idChangeEnd );
	seqCommon.extractPadding         ( nums, idChangeEnd );
	seqCommon.extractIsStrictPadding ( nums, idChangeEnd, seqCommon._padding );
	result.push_back( seqCommon );
	return result;
    }
    // it's a multi-sequence...
    const FileNumbers* previous = &nums.front();
    Sequence           s        = seqCommon;
    s._prefix += previous->getString( idChangeBegin );
    for ( std::size_t i = idChangeBegin + 1; i < idChangeEnd; ++i )
    {
	s._prefix += id[i];
	s._prefix += previous->getString( i );
    }
    s._prefix += id[idChangeEnd];
    result.push_back(s);
    std::list<Time>        times;
    std::list<std::string> timesStr;
    std::size_t            iCurrent = 0;

    BOOST_FOREACH( const FileNumbers& sn, nums )
    {
	if (!sn.rangeEquals( *previous, idChangeBegin, idChangeEnd ) )
	{
	    // update the last added sequence
	    result.back()._nbFiles   = times.size();
	    result.back()._firstTime = times.front();
	    result.back()._lastTime  = times.back();
	    result.back().extractStep            ( times );
	    result.back().extractPadding         ( timesStr );
	    result.back().extractIsStrictPadding ( timesStr, result.back()._padding );
	    times.clear();

	    // create a new sequence initilized with "sn" values
	    s = seqCommon;
	    s._prefix += sn.getString(idChangeBegin);
	    for ( std::size_t i = idChangeBegin + 1; i < idChangeEnd; ++i )
	    {
		s._prefix += id[i];
		s._prefix += sn.getString(i);
	    }
	    s._prefix += id[idChangeEnd];
	    s._padding = sn.getPadding(idChangeEnd);
	    result.push_back(s);
	    previous = &sn;
	}
	times.push_back(sn.getTime(idChangeEnd));
	timesStr.push_back(sn.getString(idChangeEnd));
	++iCurrent;
    }

    // update the last added sequence
    result.back()._nbFiles       = times.size ();
    result.back()._firstTime     = times.front();
    result.back()._lastTime      = times.back ();

    result.back().extractStep            ( times );
    result.back().extractPadding         ( timesStr );
    result.back().extractIsStrictPadding ( timesStr, result.back()._padding );

    return result;
}

/**
 * @brief Construct id and nums from a filename.
 * @param[in] str: the string to process (filename)
 * @param[out] id: list of strings
 * @param[out] nums: list of integers
 * @return number of decteted numbers
 */
std::size_t Detector::seqConstruct(const std::string& str, FileStrings& id, FileNumbers& nums)
{
    static const std::size_t     max = std::numeric_limits<std::size_t>::digits10;
    static const boost::regex    re ( "[\\-\\+]?\\d*?\\d{1," + boost::lexical_cast<std::string>(max) + "}" );
    static const int             subs[] = { -1, 0, }; // get before match and current match
    boost::sregex_token_iterator m ( str.begin(), str.end(), re, subs );
    boost::sregex_token_iterator end;

    while (m != end) {
	// begin with string id, can be an empty string if str begins with a number
	id.getId().push_back(*m++);
	if (m != end) { // if end with a string and not a number
	    nums.push_back(*m++);
	}
    }
    if (id.getId().size() == nums.size()) {
	id.getId().push_back(""); // we end with an empty string
    }
    return nums.size();
}

}
