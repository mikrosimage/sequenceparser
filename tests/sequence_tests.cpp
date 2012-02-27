#include <sequence/Range.h>
#include <sequence/Sequence.h>
#include <sequence/BrowseItem.h>

#include <map>

#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE Sequence
#include <boost/test/unit_test.hpp>

using namespace sequence;
using namespace std;

BOOST_AUTO_TEST_SUITE( RangeTestSuite )

BOOST_AUTO_TEST_CASE( empty_range )
{
    Range range;
    BOOST_CHECK_EQUAL(0u, range.first);
    BOOST_CHECK_EQUAL(0u, range.last);
}

BOOST_AUTO_TEST_CASE( range_ctor )
{
    Range range(1,2);
    BOOST_CHECK_EQUAL(1u, range.first);
    BOOST_CHECK_EQUAL(2u, range.last);

    // 'last' must be greater or equal to 'first'
    BOOST_CHECK_THROW( Range(1,0), logic_error );
}

BOOST_AUTO_TEST_CASE( range_contains )
{
    Range range(1,2);
    BOOST_CHECK( !range.contains(0) );
    BOOST_CHECK( range.contains(1) );
    BOOST_CHECK( range.contains(2) );
    BOOST_CHECK( !range.contains(3) );
}

BOOST_AUTO_TEST_CASE( range_valid )
{
    Range range;

    range.first = 10;
    BOOST_CHECK( !range.valid() ); // [10,0] is not a valid range

    range.last = 10;
    BOOST_CHECK( range.valid() );// [10,10] is a valid range
}

BOOST_AUTO_TEST_CASE( range_duration )
{
    Range range;
    BOOST_CHECK_EQUAL(1u, range.duration());

    range = Range(0,9);
    BOOST_CHECK_EQUAL(10u, range.duration());

    range = Range(10,10);
    BOOST_CHECK_EQUAL(1u, range.duration());
}

BOOST_AUTO_TEST_CASE( range_offsets )
{
    Range range(5,10);

    BOOST_CHECK_EQUAL( 7u, range.offsetClampFrame(7, 0) ); // no offset
    BOOST_CHECK_EQUAL( 8u, range.offsetClampFrame(7, 1) );// still in range
    BOOST_CHECK_EQUAL( 6u, range.offsetClampFrame(7,-1) );// still in range
    BOOST_CHECK_EQUAL(10u, range.offsetClampFrame(7, 9) );// out of range
    BOOST_CHECK_EQUAL( 5u, range.offsetClampFrame(7,-9) );// out of range

    BOOST_CHECK_EQUAL( 7u, range.offsetLoopFrame(7, 0) );// no offset
    BOOST_CHECK_EQUAL( 8u, range.offsetLoopFrame(7, 1) );// still in range
    BOOST_CHECK_EQUAL( 6u, range.offsetLoopFrame(7,-1) );// still in range
    BOOST_CHECK_EQUAL( 5u, range.offsetLoopFrame(7, 4) );// out of range
    BOOST_CHECK_EQUAL(10u, range.offsetLoopFrame(7,-3) );// out of range
}

BOOST_AUTO_TEST_CASE( range_offsets2 )
{
    Range range(1,1);
    BOOST_CHECK_EQUAL( 1u, range.offsetLoopFrame(1, 1) );
    BOOST_CHECK_EQUAL( 1u, range.offsetClampFrame(1, 1) );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( InterpolationTestSuite )

BOOST_AUTO_TEST_CASE( same_ranges )
{
    Range source(0,2);

    // forward
    BOOST_CHECK_EQUAL( 0u, interpolateSource(0, source, source, false) );
    BOOST_CHECK_EQUAL( 1u, interpolateSource(1, source, source, false) );
    BOOST_CHECK_EQUAL( 2u, interpolateSource(2, source, source, false) );

    // reverse
    BOOST_CHECK_EQUAL( 2u, interpolateSource(0, source, source, true) );
    BOOST_CHECK_EQUAL( 1u, interpolateSource(1, source, source, true) );
    BOOST_CHECK_EQUAL( 0u, interpolateSource(2, source, source, true) );
}

BOOST_AUTO_TEST_CASE( offset_ranges )
{
    Range source(2,4);
    Range record(0,2);

    // forward
    BOOST_CHECK_EQUAL( 2u, interpolateSource(0, source, record, false) );
    BOOST_CHECK_EQUAL( 3u, interpolateSource(1, source, record, false) );
    BOOST_CHECK_EQUAL( 4u, interpolateSource(2, source, record, false) );

    // reverse
    BOOST_CHECK_EQUAL( 4u, interpolateSource(0, source, record, true) );
    BOOST_CHECK_EQUAL( 3u, interpolateSource(1, source, record, true) );
    BOOST_CHECK_EQUAL( 2u, interpolateSource(2, source, record, true) );
}

BOOST_AUTO_TEST_CASE( scaled_ranges )
{
    Range source(0,10);
    Range record(0,2);

    // forward
    BOOST_CHECK_EQUAL( 0u, interpolateSource(0, source, record, false) );
    BOOST_CHECK_EQUAL( 5u, interpolateSource(1, source, record, false) );
    BOOST_CHECK_EQUAL(10u, interpolateSource(2, source, record, false) );

    // reverse
    BOOST_CHECK_EQUAL(10u, interpolateSource(0, source, record, true) );
    BOOST_CHECK_EQUAL( 5u, interpolateSource(1, source, record, true) );
    BOOST_CHECK_EQUAL( 0u, interpolateSource(2, source, record, true) );
}

BOOST_AUTO_TEST_CASE( subsampling_ranges )
{
    Range source(5,8);
    const unsigned int count = 50;
    Range record(1,4*count); // duration = 200

    // the pattern should be
    // 5555...6666...7777...8888...
    // each individual one repeated 50 times for both forward and reverse traversal

    { // forward
        map<unsigned int, size_t> index;
        for(size_t i = record.first;i<=record.last;++i)
        ++index[ interpolateSource(i, source, record, false) ];
        BOOST_CHECK_EQUAL(4u, index.size() );
        for(size_t i = source.first;i<=source.last;++i)
        BOOST_CHECK_EQUAL(count, index[i] );
    }
    { // reverse
        map<unsigned int, size_t> index;
        for(size_t i = record.first;i<=record.last;++i)
        ++index[ interpolateSource(i, source, record, true) ];
        BOOST_CHECK_EQUAL(4u, index.size() );
        for(size_t i = source.first;i<=source.last;++i)
        BOOST_CHECK_EQUAL(count, index[i] );
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( SequenceTestSuite )

BOOST_AUTO_TEST_CASE( parse_pattern_test )
{
    BOOST_CHECK_THROW( parsePattern(""), std::runtime_error );
    BOOST_CHECK_THROW( parsePattern("no_pattern_inside_me"), std::runtime_error );

    {
        const SequencePattern pattern = parsePattern("prefix#suffix");
        BOOST_CHECK_EQUAL( 1U, pattern.padding );
        BOOST_CHECK_EQUAL( "prefix", pattern.prefix );
        BOOST_CHECK_EQUAL( "suffix", pattern.suffix );
    }
    {
        const SequencePattern pattern = parsePattern("####");
        BOOST_CHECK_EQUAL( 4U, pattern.padding );
        BOOST_CHECK( pattern.prefix.empty() );
        BOOST_CHECK( pattern.suffix.empty() );
    }
}

BOOST_AUTO_TEST_CASE( instanciate_pattern_test )
{
    { // no prefix / suffix
        const SequencePattern pattern("", "",1);
        BOOST_CHECK_EQUAL( "1", instanciatePattern(pattern,1));
    }
    { // no padding
        const SequencePattern pattern("p", "s",1);
        BOOST_CHECK_EQUAL( "p100s", instanciatePattern(pattern,100));
    }
    { // 1000 can't fit in a pattern of size 2
        const SequencePattern pattern("", "",2);
        BOOST_CHECK_THROW( instanciatePattern(pattern,1000), runtime_error );
    }
    { // padding 3
        const SequencePattern pattern("prefix", "suffix",5);
        BOOST_CHECK_EQUAL( "prefix00018suffix", instanciatePattern(pattern,18));
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( BrowseItemTestSuite )

BOOST_AUTO_TEST_CASE( browse_item_test )
{
    {
        BOOST_CHECK_EQUAL( UNDEFINED, BrowseItem().type );
    }
    {
        const BrowseItem item = create_folder("folder");
        BOOST_CHECK_EQUAL( FOLDER, item.type );
        BOOST_CHECK_EQUAL( "folder", item.path );
    }
    {
        const BrowseItem item = create_file("file");
        BOOST_CHECK_EQUAL( UNITFILE, item.type );
        BOOST_CHECK_EQUAL( "file", item.path );
    }
    {
        const BrowseItem item = create_sequence("path", SequencePattern("pre","suf",3), Range(0,10));
        BOOST_CHECK_EQUAL( SEQUENCE, item.type );
        BOOST_CHECK_EQUAL( "path", item.path );
        BOOST_CHECK_EQUAL( "pre", item.sequence.pattern.prefix );
        BOOST_CHECK_EQUAL( "suf", item.sequence.pattern.suffix );
        BOOST_CHECK_EQUAL( 3U, item.sequence.pattern.padding );
        BOOST_CHECK_EQUAL( 0U , item.sequence.range.first );
        BOOST_CHECK_EQUAL( 10U , item.sequence.range.last );
        BOOST_CHECK_EQUAL( 1U , item.sequence.step );
    }
}

BOOST_AUTO_TEST_SUITE_END()
