#include <sequence/parser/ParserUtils.h>
#include <sequence/parser/impl/DefaultNumberLocationChooser.h>

#include <boost/filesystem/path.hpp>

#include <sstream>
#include <ostream>

#define BOOST_TEST_MODULE Parser
#include <boost/test/unit_test.hpp>

using namespace sequence::parser;
using namespace std;

static inline void check_equals(const NumberLocation &a, const NumberLocation &b) {
    BOOST_CHECK_EQUAL(a.first, b.first);
    BOOST_CHECK_EQUAL(a.count, b.count);
}

BOOST_AUTO_TEST_SUITE( ParsingSuite )

BOOST_AUTO_TEST_CASE( PatternKeyTest )
{
    PatternKey pk("a023bc1d456");
    BOOST_CHECK_EQUAL( pk.key, "a###bc#d###" );
    BOOST_CHECK_EQUAL( pk.locationCount(), 3u );
    BOOST_CHECK_EQUAL( pk.fileCount(), 1u );
    check_equals( pk.numberLocations[0], NumberLocation(1,3) );
    check_equals( pk.numberLocations[1], NumberLocation(6,1) );
    check_equals( pk.numberLocations[2], NumberLocation(8,3) );
    BOOST_CHECK_EQUAL( pk.values.size(), 3u );
    BOOST_CHECK_EQUAL( pk.values[0], 23u );
    BOOST_CHECK_EQUAL( pk.values[1], 1u );
    BOOST_CHECK_EQUAL( pk.values[2], 456u );
}

BOOST_AUTO_TEST_CASE( TooMuchNumberTest )
{
    const string filename( "0.1.2.3.4.5.6.7.8.9" );
    PatternKey pk(filename);
    BOOST_CHECK_EQUAL( pk.key, filename );
    BOOST_CHECK( pk.values.empty() );
    BOOST_CHECK( pk.numberLocations.empty() );
}

BOOST_AUTO_TEST_CASE( AggregatorTest )
{
    PatternAggregator aggregator;
    aggregator("path/p2.sgi");
    aggregator("path/p3.sgi");

    const FolderMap &folderMap = aggregator.getFolderMap();
    BOOST_CHECK_EQUAL( folderMap.size() ,1u );
    FolderMap::const_iterator itr = folderMap.find("path");
    BOOST_CHECK( itr != aggregator.getFolderMap().end() );
    const KeyMap &map = itr->second;
    const PatternKey &key = map.begin()->second;
    BOOST_CHECK_EQUAL( key.locationCount(), 1u );
    BOOST_CHECK_EQUAL( key.fileCount(), 2u );
    BOOST_CHECK_EQUAL( key.key,"p#.sgi");
    check_equals(key.numberLocations[0], NumberLocation(1,1));
    BOOST_CHECK_EQUAL( key.values.size(),2u );
    BOOST_CHECK_EQUAL( key.values[0],2u );
    BOOST_CHECK_EQUAL( key.values[1],3u );
}

//BOOST_AUTO_TEST_CASE( RemoveTrivialLocationTest )
//{
//    PatternKey pk("image1-000.cr2");
//    pk.append(PatternKey("image1-001.cr2"));
//    pk.append(PatternKey("image1-002.cr2"));
//    pk.append(PatternKey("image1-003.cr2"));
//
//    BOOST_CHECK_EQUAL( pk.locationCount(), 3u );
//    removeConstantLocations(pk);
//    BOOST_CHECK_EQUAL( pk.locationCount(), 1u );
//}

BOOST_AUTO_TEST_CASE( ChooseLocationDefaultTest )
{
    PatternKey pk("_023__1_456");
    BOOST_CHECK_EQUAL( pk.locationCount(), 3u );

    // only one file, it can't be a sequence
    BOOST_CHECK_EQUAL( chooseNumberLocation(pk), (NumberLocation*) NULL );

    // two file, we can tell which one is moving
    pk.append(PatternKey("_023__2_456"));
    BOOST_CHECK_EQUAL( chooseNumberLocation(pk), &pk.numberLocations[1] );

    // three files now but we can't draw any conclusions
    pk.append(PatternKey("_023__1_466"));
    BOOST_CHECK_EQUAL( chooseNumberLocation(pk), (NumberLocation*) NULL );
}
BOOST_AUTO_TEST_SUITE_END()
