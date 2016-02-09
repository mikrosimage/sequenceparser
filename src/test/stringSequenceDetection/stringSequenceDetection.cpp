#define BOOST_TEST_MODULE StringSequenceDetector

#include <test/common.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/test/unit_test.hpp>

using boost::unit_test::test_suite;

BOOST_AUTO_TEST_SUITE( SimpleDetections )

BOOST_AUTO_TEST_CASE( Nothing )
{
	std::vector<boost::filesystem::path> paths;

	std::vector<sequenceParser::Sequence> listSequence = sequenceFromFilenameList(paths);
	BOOST_CHECK_EQUAL( listSequence.size(), 0 );
}

BOOST_AUTO_TEST_CASE( SingleFilename )
{
	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2.j2c" )
		;

	std::vector<sequenceParser::Sequence> listSequence = sequenceFromFilenameList(paths);
	BOOST_CHECK_EQUAL( listSequence.size(), 0 );
}

BOOST_AUTO_TEST_CASE( SingleSequence )
{
	std::vector<sequenceParser::Sequence> listSequence1;

	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			;

		listSequence1 = sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence1.front().getFrameRanges().size(), 1 );
	}

	std::vector<sequenceParser::Sequence> listSequence2;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;

		listSequence2 = sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence2.size(), 1 );
	}

	BOOST_CHECK_EQUAL( listSequence1.front(), listSequence2.front() );

	std::vector<sequenceParser::Sequence> listSequence3;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b5.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;

		listSequence3 = sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence2.size(), 1 );
	}

	BOOST_CHECK_NE( listSequence1.front(), listSequence3.front() );
}

BOOST_AUTO_TEST_CASE( SingleSequence_multiRange )
{
	std::vector<sequenceParser::Sequence> listSequence1;

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

	listSequence1 = sequenceFromFilenameList( paths );
	
	BOOST_CHECK_EQUAL( listSequence1.size(), 1 );
	BOOST_CHECK_EQUAL( listSequence1.front().getFrameRanges().size(), 3 );
		
	std::cout << "Iterate" << std::endl;
	using namespace boost::assign;
	{
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
	{
                std::vector<sequenceParser::Time> subtimes;
		subtimes += 14, 15, 16;

		std::vector<sequenceParser::Time>::const_iterator sit = subtimes.begin();
		BOOST_FOREACH(sequenceParser::Time t, listSequence1.front().getFramesIterable(6, 17))
		{
			std::cout << "t: " << t << std::endl;
			BOOST_CHECK_EQUAL( t, *sit++ );
		}
		BOOST_CHECK( sit == subtimes.end() );
	}
}

BOOST_AUTO_TEST_SUITE_END()

