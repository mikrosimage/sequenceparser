#include <Detector.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( MultiPaddingSequenceDetection )

BOOST_AUTO_TEST_CASE( SequenceWithPadding )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c001.j2c" )
		( "aaa/bbb/a1b2c002.j2c" )
		( "aaa/bbb/a1b2c003.j2c" )
		( "aaa/bbb/a1b2c809.j2c" )
		( "aaa/bbb/a1b2c999.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths );

	BOOST_CHECK( listSequence.size() == 1 );
	
	const boost::shared_ptr<sequenceParser::Sequence> seq = listSequence.front();
	BOOST_CHECK( seq->getPadding() == 3 );
	BOOST_CHECK( seq->isStrictPadding() == true );
}

BOOST_AUTO_TEST_CASE( SequenceWithoutPadding )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c99.j2c" )
		( "aaa/bbb/a1b2c102.j2c" )
		( "aaa/bbb/a1b2c1234.j2c" )
		( "aaa/bbb/a1b2c12345.j2c" )
		( "aaa/bbb/a1b2c123456.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths );

	std::cout << "AA: " << listSequence.size() << std::endl;
	BOOST_CHECK( listSequence.size() == 1 );

	const boost::shared_ptr<sequenceParser::Sequence> seq = listSequence.front();
	BOOST_CHECK( seq->getPadding() == 0 );
	BOOST_CHECK( seq->getFirstTime() == 99 );
	BOOST_CHECK( seq->getLastTime() == 123456 );
	BOOST_CHECK( seq->getNbFiles() == 5 );
	BOOST_CHECK( seq->hasMissingFile() == true );
	std::cout << "seq->getStep(): " << seq->getStep() << std::endl;
	BOOST_CHECK( seq->getStep() == 1 );
}

BOOST_AUTO_TEST_CASE( DoublePaddingForTheSameSequence )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::Sequence > > listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c00099.j2c" )
		( "aaa/bbb/a1b2c00102.j2c" )
	
		( "aaa/bbb/a1b2c1234.j2c" )
		( "aaa/bbb/a1b2c12345.j2c" )
		( "aaa/bbb/a1b2c123456.j2c" )
		;

	listSequence = detector.sequenceFromFilenameList( paths );

	std::cout << "AA: " << listSequence.size() << std::endl;
//	BOOST_CHECK( listSequence.size() == 2 ); // @todo bug to fix !
}


BOOST_AUTO_TEST_SUITE_END()
