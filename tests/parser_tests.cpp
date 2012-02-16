#include <sequence/parser/PatternAggregator.h>

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
    PatternKey pk("a023bc1d456");
    BOOST_CHECK_EQUAL( pk.key, "a###bc#d###" );
    BOOST_CHECK_EQUAL( pk.size(), 3 );
    check_equals( pk.locations[0], Location(1,3) );
    check_equals( pk.locations[1], Location(6,1) );
    check_equals( pk.locations[2], Location(8,3) );
    BOOST_CHECK_EQUAL( pk.values.size(), 3 );
    BOOST_CHECK_EQUAL( pk.values[0], 23 );
    BOOST_CHECK_EQUAL( pk.values[1], 1 );
    BOOST_CHECK_EQUAL( pk.values[2], 456 );
}

BOOST_AUTO_TEST_CASE( AggregatorTest )
{
    PatternAggregator aggregator;
    aggregator("path/p2.sgi");
    aggregator("path/p3.sgi");
    BOOST_CHECK_EQUAL( aggregator.folderMap.size() ,1 );
    PatternAggregator::KeyMap &map =aggregator.folderMap["path"];
    BOOST_CHECK_EQUAL( map.size() ,1 );
    const PatternKey &key = map.begin()->second;
    BOOST_CHECK_EQUAL( key.key,"p#.sgi");
    check_equals(key.locations[0], Location(1,1));
    BOOST_CHECK_EQUAL( key.values.size(),2 );
    BOOST_CHECK_EQUAL( key.values[0],2 );
    BOOST_CHECK_EQUAL( key.values[1],3 );
}

BOOST_AUTO_TEST_SUITE_END()
