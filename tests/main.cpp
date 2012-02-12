#define BOOST_TEST_MODULE Sequence
#include <boost/test/unit_test.hpp>

#include <sequence/Range.h>

using namespace sequence;

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
	BOOST_CHECK_THROW( Range(1,0), std::logic_error );
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

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( InterpolationTestSuite )

BOOST_AUTO_TEST_CASE( same_ranges )
{
	Range source(0,2);

	// forward
	BOOST_CHECK_EQUAL( 0u, srcFromRec(source, source, 0, false) );
	BOOST_CHECK_EQUAL( 1u, srcFromRec(source, source, 1, false) );
	BOOST_CHECK_EQUAL( 2u, srcFromRec(source, source, 2, false) );

	// reverse
	BOOST_CHECK_EQUAL( 2u, srcFromRec(source, source, 0, true) );
	BOOST_CHECK_EQUAL( 1u, srcFromRec(source, source, 1, true) );
	BOOST_CHECK_EQUAL( 0u, srcFromRec(source, source, 2, true) );
}

BOOST_AUTO_TEST_CASE( offset_ranges )
{
	Range source(2,4);
	Range record(0,2);

	// forward
	BOOST_CHECK_EQUAL( 2u, srcFromRec(source, record, 0, false) );
	BOOST_CHECK_EQUAL( 3u, srcFromRec(source, record, 1, false) );
	BOOST_CHECK_EQUAL( 4u, srcFromRec(source, record, 2, false) );

	// reverse
	BOOST_CHECK_EQUAL( 4u, srcFromRec(source, record, 0, true) );
	BOOST_CHECK_EQUAL( 3u, srcFromRec(source, record, 1, true) );
	BOOST_CHECK_EQUAL( 2u, srcFromRec(source, record, 2, true) );
}

BOOST_AUTO_TEST_CASE( scaled_ranges )
{
	Range source(0,10);
	Range record(0,2);

	// forward
	BOOST_CHECK_EQUAL( 0u, srcFromRec(source, record, 0, false) );
	BOOST_CHECK_EQUAL( 5u, srcFromRec(source, record, 1, false) );
	BOOST_CHECK_EQUAL(10u, srcFromRec(source, record, 2, false) );

	// reverse
	BOOST_CHECK_EQUAL(10u, srcFromRec(source, record, 0, true) );
	BOOST_CHECK_EQUAL( 5u, srcFromRec(source, record, 1, true) );
	BOOST_CHECK_EQUAL( 0u, srcFromRec(source, record, 2, true) );
}

BOOST_AUTO_TEST_CASE( complex_ranges )
{
	Range source(5,8);
	Range record(0,150);

	for(size_t i = record.first;i<=record.last;++i)
		std::cout << srcFromRec(source, record, i, false);
	std::cout << std::endl;
	for(size_t i = record.first;i<=record.last;++i)
		std::cout << srcFromRec(source, record, i, true);
}

BOOST_AUTO_TEST_SUITE_END()
