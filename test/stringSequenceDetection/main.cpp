#include <Detector.hpp>

#include <boost/assign/std/vector.hpp>

#define BOOST_TEST_MODULE StringSequenceDetector

#include <boost/test/unit_test.hpp>
using boost::unit_test::test_suite;

BOOST_AUTO_TEST_SUITE( SimpleDetections )

BOOST_AUTO_TEST_CASE( Nothing )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	listSequence = detector.sequenceFromFilenameList( paths );

	BOOST_CHECK( listSequence.size() == 0 );
}

BOOST_AUTO_TEST_CASE( SingleFilename )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths );

	BOOST_CHECK( listSequence.size() == 0 );
}

BOOST_AUTO_TEST_CASE( SingleSequence )
{
	sequenceParser::Detector detector;
	std::list< boost::shared_ptr<sequenceParser::Sequence> > listSequence1;

	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			;

		listSequence1 = detector.sequenceFromFilenameList( paths );
	}

	std::list< boost::shared_ptr<sequenceParser::Sequence> > listSequence2;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b4.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;
		listSequence2 = detector.sequenceFromFilenameList( paths );
		BOOST_CHECK( listSequence2.size() == 1 );
	}

	BOOST_CHECK( *( listSequence1.front() ) == *( listSequence2.front() ) );

	std::list< boost::shared_ptr<sequenceParser::Sequence> > listSequence3;
	{
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2.j2c" )
			( "aaa/bbb/a1b5.j2c" )
			( "aaa/bbb/a1b3.j2c" )
			;
		listSequence3 = detector.sequenceFromFilenameList( paths );
		BOOST_CHECK( listSequence2.size() == 1 );
	}

	BOOST_CHECK( *( listSequence1.front() ) != *( listSequence3.front() ) );
}


BOOST_AUTO_TEST_SUITE_END()

