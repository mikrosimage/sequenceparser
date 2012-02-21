#include <sequence/parser/Browser.h>
#include <sequence/parser/ParserUtils.h>
#include <sequence/DisplayUtils.h>

#include <boost/filesystem/path.hpp>
#include <boost/assign/std/set.hpp>
#include <boost/assign/list_of.hpp>

#include <sstream>
#include <ostream>

#define BOOST_TEST_MODULE Parser
#include <boost/test/unit_test.hpp>

using namespace sequence;
using namespace sequence::parser::details;
using namespace std;

static inline void check_equals(const Location &a, const Location &b) {
    BOOST_CHECK_EQUAL(a.first, b.first);
    BOOST_CHECK_EQUAL(a.count, b.count);
}

static inline void check_equals(const Range &a, const Range &b) {
    BOOST_CHECK_EQUAL(a.first, b.first);
    BOOST_CHECK_EQUAL(a.last, b.last);
}

BOOST_AUTO_TEST_SUITE( ParsingSuite )

BOOST_AUTO_TEST_CASE( LocationValueSetGetStepTest )
{
    unsigned step;
    {
        LocationValueSet set;
        boost::assign::insert(set)(0)(1)(2)(3)(4);
        BOOST_CHECK( !set.isConstant() );
        std::vector<Range> ranges = set.getConsecutiveRanges(step);
        BOOST_CHECK_EQUAL( step, 1u );
        BOOST_CHECK_EQUAL( ranges.size(), 1u );
        check_equals( ranges[0], Range(0,4));
    }
    {
        LocationValueSet set;
        boost::assign::insert(set)(1)(2)(5)(6)(11)(12)(13)(14)(20)(22)(24)(26)(28)(30)(34)(36);
        BOOST_CHECK( !set.isConstant() );
        std::vector<Range> ranges = set.getConsecutiveRanges(step);
        BOOST_CHECK_EQUAL( step, 1u );
        BOOST_CHECK_EQUAL( ranges.size(), 11u );
        check_equals( ranges[0], Range(1,2));
        check_equals( ranges[1], Range(5,6));
        check_equals( ranges[2], Range(11,14));
        check_equals( ranges[3], Range(20,20));
        check_equals( ranges[4], Range(22,22));
        check_equals( ranges[5], Range(24,24));
        check_equals( ranges[6], Range(26,26));
        check_equals( ranges[7], Range(28,28));
        check_equals( ranges[8], Range(30,30));
        check_equals( ranges[9], Range(34,34));
        check_equals( ranges[10], Range(36,36));
    }
    {
        LocationValueSet set;
        boost::assign::insert(set)(20)(22)(24)(26)(28)(30)(34)(36);
        BOOST_CHECK( !set.isConstant() );
        std::vector<sequence::Range> ranges = set.getConsecutiveRanges(step);
        BOOST_CHECK_EQUAL( step, 2u );
        BOOST_CHECK_EQUAL( ranges.size(), 2u );
        check_equals( ranges[0], Range(20,30));
        check_equals( ranges[1], Range(34,36));
    }
    {
        LocationValueSet set;
        boost::assign::insert(set)(1);
        BOOST_CHECK( set.isConstant() );
    }
}

BOOST_AUTO_TEST_CASE( PatternKeyTest )
{
    string key("a023bc1d456");
    Locations locations;
    Values values;
    extractPattern(key, locations, values);
    BOOST_CHECK_EQUAL( key, "a###bc#d###" );
    BOOST_CHECK_EQUAL( locations.size(), 3u );
    check_equals( locations[0], Location(1,3) );
    check_equals( locations[1], Location(6,1) );
    check_equals( locations[2], Location(8,3) );
    BOOST_CHECK_EQUAL( values.size(), 3u );
    BOOST_CHECK_EQUAL( values[0], 23u );
    BOOST_CHECK_EQUAL( values[1], 1u );
    BOOST_CHECK_EQUAL( values[2], 456u );
}

BOOST_AUTO_TEST_CASE( AggregatorTest )
{
    SequenceDetector map;
    map("p2.sgi");
    map("p3.sgi");

    const PatternAggregator &va = map.begin()->second;
    BOOST_CHECK_EQUAL( va.locationCount(), 1u );
    BOOST_CHECK_EQUAL( va.fileCount(), 2u );
    BOOST_CHECK_EQUAL( va.key,"p#.sgi");
    check_equals(va.locations[0], Location(1,1));
    BOOST_CHECK_EQUAL( va.size(),2u );
    BOOST_CHECK_EQUAL( va[0],2u );
    BOOST_CHECK_EQUAL( va[1],3u );
}

BOOST_AUTO_TEST_CASE( ValueAggregatorSetsTest )
{
    SequenceDetector parser;
    const PatternAggregator &va = parser("p2.cr2");
    const PatternAggregator::LocationValueSets &sets = va.getValueSets();
    BOOST_CHECK_EQUAL( sets.size(),2u );
    BOOST_CHECK_EQUAL( sets[0].size(),1u );
    BOOST_CHECK_EQUAL( sets[1].size(),1u );
    parser("p3.cr2");
    BOOST_CHECK_EQUAL( sets[0].size(),2u );
    BOOST_CHECK_EQUAL( sets[1].size(),1u );
    parser("p3.cr1");
    BOOST_CHECK_EQUAL( sets[0].size(),2u );
    BOOST_CHECK_EQUAL( sets[1].size(),2u );
}

BOOST_AUTO_TEST_CASE( SimplifyingTest )
{
    SequenceDetector parser;
    const PatternAggregator &va = parser("0_0_012");
    parser("0_1_012");
    parser("0_2_012");
    {
        BOOST_CHECK_EQUAL( va.key , "#_#_###" );
        const PatternAggregator::LocationValueSets &sets = va.getValueSets();
        BOOST_CHECK_EQUAL( sets.size(),3u );
        BOOST_CHECK_EQUAL( sets[0].size(),1u );
        BOOST_CHECK_EQUAL( sets[1].size(),3u );
        BOOST_CHECK_EQUAL( sets[2].size(),1u );
    }
    {
        const PatternAggregator another = va.removeConstantLocations();
        BOOST_CHECK_EQUAL( another.key,"0_#_012" );
        const PatternAggregator::LocationValueSets &sets = another.getValueSets();
        BOOST_CHECK_EQUAL( sets.size(),1u );
        BOOST_CHECK_EQUAL( sets[0].size(),3u );
    }
}

bool less(const BrowseItem&a, const BrowseItem&b) {
    ostringstream a_out,b_out;
    a_out << a;
    b_out << b;
    return a_out.str()<b_out.str();
}

BOOST_AUTO_TEST_CASE( FinalizeTest )
{
    using sequence::BrowseItem;
    SequenceDetector parser("path");
    parser("afile.txt");
    parser("file_with_numbers_0213.txt");
    parser("_00132_file11.cr2");
    parser("_00132_file12.cr2");
    parser("_00132_file13.cr2");
    parser("p13.cr2");
    parser("p18.cr2");
    parser("p23.cr2");
    parser("p28.cr2");
    std::vector<BrowseItem> items = parser.getResults();
    sort(items.begin(), items.end(), &less);
// the result will be sorted lexicographically
    {
        const BrowseItem &item = items[0];
        BOOST_CHECK_EQUAL( item.type, SEQUENCE);
        BOOST_CHECK_EQUAL( item.path, "path");
        BOOST_CHECK_EQUAL( item.sequence.pattern.prefix, "_00132_file");
        BOOST_CHECK_EQUAL( item.sequence.pattern.suffix, ".cr2");
        BOOST_CHECK_EQUAL( item.sequence.pattern.padding, 2u);
        BOOST_CHECK_EQUAL( item.sequence.range.first, 11u);
        BOOST_CHECK_EQUAL( item.sequence.range.last, 13u);
        BOOST_CHECK_EQUAL( item.sequence.step, 1u);
    }
    {
        const BrowseItem &item = items[1];
        BOOST_CHECK_EQUAL( item.type, SEQUENCE);
        BOOST_CHECK_EQUAL( item.path, "path");
        BOOST_CHECK_EQUAL( item.sequence.pattern.prefix, "p");
        BOOST_CHECK_EQUAL( item.sequence.pattern.suffix, ".cr2");
        BOOST_CHECK_EQUAL( item.sequence.pattern.padding, 2u);
        BOOST_CHECK_EQUAL( item.sequence.range.first, 13u);
        BOOST_CHECK_EQUAL( item.sequence.range.last, 28u);
        BOOST_CHECK_EQUAL( item.sequence.step, 5u);
    }
    {
        const BrowseItem &item = items[2];
        BOOST_CHECK_EQUAL( item.type, UNITFILE);
        BOOST_CHECK_EQUAL( item.path, "path/afile.txt");
    }
    {
        const BrowseItem &item = items[3];
        BOOST_CHECK_EQUAL( item.type, UNITFILE);
        BOOST_CHECK_EQUAL( item.path, "path/file_with_numbers_0213.txt");
    }
//    copy(items.begin(), items.end(), ostream_iterator<sequence::BrowseItem>(cout, "\n"));
}

BOOST_AUTO_TEST_CASE( TrickyTestCaseWithSeveralIncrementingCounter )
{
    using sequence::BrowseItem;
    SequenceDetector parser;
    parser("_1_1_");
    parser("_1_2_");
    parser("_2_2_");
    parser("_2_1_");
    const std::vector<BrowseItem> items = parser.getResults();
}

BOOST_AUTO_TEST_SUITE_END()
