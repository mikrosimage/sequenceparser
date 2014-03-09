#include <detector.hpp>

#include <boost/assign/std/vector.hpp>

#define BOOST_TEST_MODULE StringSequenceDetector

#include <boost/test/unit_test.hpp>
using boost::unit_test::test_suite;

BOOST_AUTO_TEST_SUITE( SimpleDetections )

BOOST_AUTO_TEST_CASE( Nothing )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 0 );
}

BOOST_AUTO_TEST_CASE( SingleFilename )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 0 );
}

BOOST_AUTO_TEST_CASE( SingleSequence )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence1;

	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			;

		listSequence1 = sequenceParser::sequenceFromFilenameList( paths );
	}

	boost::ptr_vector<sequenceParser::Sequence> listSequence2;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;
		listSequence2 = sequenceParser::sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence2.size(), 1 );
	}

	BOOST_CHECK_EQUAL( listSequence1.front(), listSequence2.front() );

	boost::ptr_vector<sequenceParser::Sequence> listSequence3;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b5.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;
		listSequence3 = sequenceParser::sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence2.size(), 1 );
	}

	BOOST_CHECK_NE( listSequence1.front(), listSequence3.front() );
}


BOOST_AUTO_TEST_SUITE_END()

