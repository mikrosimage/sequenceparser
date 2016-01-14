#include <detector.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( MultiPaddingSequenceDetection )

BOOST_AUTO_TEST_CASE( SequenceWithPadding1 )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c001.j2c" )
		( "aaa/bbb/a1b2c002.j2c" )
		( "aaa/bbb/a1b2c003.j2c" )
		( "aaa/bbb/a1b2c023.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 1 );
	
	const sequenceParser::Sequence& seq = listSequence.front();
	BOOST_CHECK_EQUAL( seq.getFixedPadding(), 3 );
	BOOST_CHECK_EQUAL( seq.isStrictPadding(), true );
}

BOOST_AUTO_TEST_CASE( SequenceWithPadding2 )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c001.j2c" )
		( "aaa/bbb/a1b2c002.j2c" )
		( "aaa/bbb/a1b2c003.j2c" )
		( "aaa/bbb/a1b2c809.j2c" )
		( "aaa/bbb/a1b2c999.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 1 );
	
	const sequenceParser::Sequence& seq = listSequence.front();
	BOOST_CHECK_EQUAL( seq.getFixedPadding(), 3 );
	BOOST_CHECK_EQUAL( seq.isStrictPadding(), true );
}

BOOST_AUTO_TEST_CASE( SequenceWithoutPadding )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c99.j2c" )
		( "aaa/bbb/a1b2c102.j2c" )
		( "aaa/bbb/a1b2c1234.j2c" )
		( "aaa/bbb/a1b2c12345.j2c" )
		( "aaa/bbb/a1b2c123456.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 1 );

	const sequenceParser::Sequence& seq = listSequence.front();
	BOOST_CHECK_EQUAL( seq.getFixedPadding(), 0 );
	BOOST_CHECK_EQUAL( seq.getFirstTime(), 99 );
	BOOST_CHECK_EQUAL( seq.getLastTime(), 123456 );
	BOOST_CHECK_EQUAL( seq.getNbFiles(), 5 );
	BOOST_CHECK_EQUAL( seq.hasMissingFile(), true );
	BOOST_CHECK_EQUAL( seq.getFrameRanges().size(), 5 );
	BOOST_CHECK_EQUAL( seq.getFrameRanges().front().step, 0 );
}

BOOST_AUTO_TEST_CASE( DoublePaddingForTheSameSequence )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c00099.j2c" )
		( "aaa/bbb/a1b2c00102.j2c" )
	
		( "aaa/bbb/a1b2c1234.j2c" )
		( "aaa/bbb/a1b2c12345.j2c" )
		( "aaa/bbb/a1b2c123456.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 2 );
}


BOOST_AUTO_TEST_SUITE_END()
