#include <Detector.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( NegativeSequenceDetection )

BOOST_AUTO_TEST_CASE( NoNegativeValues )
{
	sequenceParser::Detector detector;
	std::vector<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b-2c1.j2c" )
		( "aaa/bbb/a1b-2c2.j2c" )
		( "aaa/bbb/a1b-2c3.j2c" )

		( "aaa/bbb/a1b+3c6.j2c" )
		( "aaa/bbb/a1b+3c2.j2c" )
		( "aaa/bbb/a1b+3c3.j2c" )

		( "aaa/bbb/a1b9c6.j2c" )
		( "aaa/bbb/a1b9c2.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths );

	BOOST_CHECK( listSequence.size() == 3 );
}

BOOST_AUTO_TEST_CASE( NegativeSequence )
{
	using namespace sequenceParser;
	Detector detector;
	std::vector<boost::shared_ptr<Sequence> > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c-3.j2c" )
		( "aaa/bbb/a1b2c-2.j2c" )
		( "aaa/bbb/a1b2c-1.j2c" )
		( "aaa/bbb/a1b2c0.j2c" )
		( "aaa/bbb/a1b2c1.j2c" )
		( "aaa/bbb/a1b2c2.j2c" )
		( "aaa/bbb/a1b2c3.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths, eMaskOptionsNegativeIndexes );

	//std::cout << "listSequence.size(): " << listSequence.size() << std::endl;
	BOOST_CHECK( listSequence.size() == 1 );

	const boost::shared_ptr<Sequence> seq = listSequence.front();
	BOOST_CHECK( seq->getFirstTime() == -3 );
	BOOST_CHECK( seq->getLastTime() == 3 );
	BOOST_CHECK( seq->getNbFiles() == 7 );
	BOOST_CHECK( seq->hasMissingFile() == false );
	BOOST_CHECK( seq->getStep() == 1 );
}


BOOST_AUTO_TEST_SUITE_END()
