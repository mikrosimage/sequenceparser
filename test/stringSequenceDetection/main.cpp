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
		
		BOOST_CHECK_EQUAL( listSequence1.front().getFrameRanges().size(), 1 );
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

BOOST_AUTO_TEST_CASE( SingleSequence_multiRange )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence1;

	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			( "aaa/bbb/a1b14.j2c" )
			( "aaa/bbb/a1b15.j2c" )
			( "aaa/bbb/a1b16.j2c" )
			( "aaa/bbb/a1b20.j2c" )
			( "aaa/bbb/a1b22.j2c" )
			( "aaa/bbb/a1b24.j2c" )
			;

		listSequence1 = sequenceParser::sequenceFromFilenameList( paths );
		
		BOOST_CHECK_EQUAL( listSequence1.front().getFrameRanges().size(), 3 );
		
		std::cout << "Iterate" << std::endl;
		using namespace boost::assign;
		std::vector<sequenceParser::Time> times;
		times += 2, 3, 4, 14, 15, 16, 20, 22, 24;

		std::vector<sequenceParser::Time>::const_iterator it = times.begin();
		BOOST_FOREACH(sequenceParser::Time t, listSequence1.front().getFramesIterable())
		{
			BOOST_CHECK_EQUAL( t, *it++ );
		}
		std::vector<sequenceParser::Time>::const_reverse_iterator rit = times.rbegin();
		std::cout << "Iterate reverse" << std::endl;
		BOOST_REVERSE_FOREACH(sequenceParser::Time t, listSequence1.front().getFramesIterable())
		{
			BOOST_CHECK_EQUAL( t, *rit++ );
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()

