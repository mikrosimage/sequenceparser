#include <sequence/parser/ParserUtils.h>
#include <sequence/DisplayUtils.h>

#include <boost/filesystem/path.hpp>

#include <sstream>
#include <ostream>

#define BOOST_TEST_MODULE Parser
#include <boost/test/unit_test.hpp>

using namespace sequence::parser;
using namespace std;

static inline void check_equals(const Location &a, const Location &b) {
    BOOST_CHECK_EQUAL(a.first, b.first);
    BOOST_CHECK_EQUAL(a.count, b.count);
}

BOOST_AUTO_TEST_SUITE( ParsingSuite )

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
    Parser parser;
    parser("path/p2.sgi");
    parser("path/p3.sgi");

    const FolderMap &folderMap = parser.getFolderMap();
    BOOST_CHECK_EQUAL( folderMap.size() ,1u );
    FolderMap::const_iterator itr = folderMap.find("path");
    BOOST_CHECK( itr != parser.getFolderMap().end() );
    const FilenameAggregator &map = itr->second;
    const PatternAggregator &va = map.begin()->second;
    BOOST_CHECK_EQUAL( va.locationCount(), 1u );
    BOOST_CHECK_EQUAL( va.files(), 2u );
    BOOST_CHECK_EQUAL( va.key,"p#.sgi");
    check_equals(va.locations[0], Location(1,1));
    BOOST_CHECK_EQUAL( va.size(),2u );
    BOOST_CHECK_EQUAL( va[0],2u );
    BOOST_CHECK_EQUAL( va[1],3u );
}

BOOST_AUTO_TEST_CASE( ValueAggregatorSetsTest )
{
    Parser parser;
    const PatternAggregator &va = parser("p2.cr2");
    const PatternAggregator::Sets &sets = va.getSets();
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
    Parser parser;
    const PatternAggregator &va = parser("0_0_012");
    parser("0_1_012");
    parser("0_2_012");
    {
        BOOST_CHECK_EQUAL( va.key , "#_#_###" );
        const PatternAggregator::Sets &sets = va.getSets();
        BOOST_CHECK_EQUAL( sets.size(),3u );
        BOOST_CHECK_EQUAL( sets[0].size(),1u );
        BOOST_CHECK_EQUAL( sets[1].size(),3u );
        BOOST_CHECK_EQUAL( sets[2].size(),1u );
    }
    {
        const PatternAggregator another = va.morphIfNeeded();
        BOOST_CHECK_EQUAL( another.key,"0_#_012" );
        const PatternAggregator::Sets &sets = another.getSets();
        BOOST_CHECK_EQUAL( sets.size(),1u );
        BOOST_CHECK_EQUAL( sets[0].size(),3u );
    }

}

BOOST_AUTO_TEST_CASE( FinalizeTest )
{
    using sequence::BrowseItem;
    Parser parser;
    parser("path/pouet.txt");
    parser("path/_00132_file11.cr2");
    parser("path/_00132_file12.cr2");
    parser("path/_00132_file13.cr2");
    const std::vector<BrowseItem> items = parser.finalize();
    copy(items.begin(), items.end(), ostream_iterator<BrowseItem>(cout , "\n"));
}

BOOST_AUTO_TEST_SUITE_END()
