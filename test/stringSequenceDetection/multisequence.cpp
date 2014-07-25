#include <detector.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( MutiSequenceDetection )

BOOST_AUTO_TEST_CASE( SimpleMultiSequence )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c1.j2c" )
		( "aaa/bbb/a1b2c2.j2c" )
		( "aaa/bbb/a1b2c3.j2c" )

		( "aaa/bbb/a1b3c6.j2c" )
		( "aaa/bbb/a1b3c2.j2c" )
		( "aaa/bbb/a1b3c3.j2c" )

		( "aaa/bbb/a1b9c6.j2c" )
		( "aaa/bbb/a1b9c2.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 3 );
}

BOOST_AUTO_TEST_CASE( SimpleMultiSequenceMultiLevel )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	std::vector<boost::filesystem::path> paths;

	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c1.j2c" )
		( "aaa/bbb/a1b2c2.j2c" )
		( "aaa/bbb/a1b2c3.j2c" )

		( "aaa/bbb/a1b3c4.j2c" )
		( "aaa/bbb/a1b4c4.j2c" )
		( "aaa/bbb/a1b5c4.j2c" )
		( "aaa/bbb/a1b6c4.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths );

	BOOST_CHECK_EQUAL( listSequence.size(), 2 );
	BOOST_CHECK_EQUAL( listSequence.front().getNbFiles(), 3 );
	BOOST_CHECK_EQUAL( listSequence.back().getNbFiles(), 4 );
}

BOOST_AUTO_TEST_CASE( MultiSequenceMultiLevelMultiPadding )
{
	{
		boost::ptr_vector<sequenceParser::Sequence> listSequence;
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2c1.j2c" )
			( "aaa/bbb/a1b002c2.j2c" ) // alone file will be ignored
			( "aaa/bbb/a1b2c3.j2c" )

			( "aaa/bbb/a1b3c4.j2c" )
			( "aaa/bbb/a1b4c4.j2c" )
			( "aaa/bbb/a1b5c4.j2c" )
			( "aaa/bbb/a1b6c4.j2c" )
			;

		listSequence = sequenceParser::sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence.size(), 2 );
	}
	{
		boost::ptr_vector<sequenceParser::Sequence> listSequence;
		std::vector<boost::filesystem::path> paths;
		boost::assign::push_back( paths )
			( "aaa/bbb/a1b2c1.j2c" )
			( "aaa/bbb/a1b002c2.j2c" )
			( "aaa/bbb/a1b2c3.j2c" )

			( "aaa/bbb/a1b002c3.j2c" )
			( "aaa/bbb/a1b3c4.j2c" )
			( "aaa/bbb/a1b4c4.j2c" )
			( "aaa/bbb/a1b5c4.j2c" )
			( "aaa/bbb/a1b6c4.j2c" )
			;

		listSequence = sequenceParser::sequenceFromFilenameList( paths );
		BOOST_CHECK_EQUAL( listSequence.size(), 3 );
	}
}

BOOST_AUTO_TEST_CASE( MultiSequenceMultiLevelMultiPaddingWithNegValues )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;
	std::vector<boost::filesystem::path> paths;
	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2c1.j2c" )
		( "aaa/bbb/a1b002c2.j2c" )
		( "aaa/bbb/a1b2c3.j2c" )

		( "aaa/bbb/a1b002c-3.j2c" )
		( "aaa/bbb/a1b3c4.j2c" )
		( "aaa/bbb/a1b5c4.j2c" )
		( "aaa/bbb/a1b6c4.j2c" )
	
		( "aaa/bbb/a1b4c-4.j2c" )
		( "aaa/bbb/a1b5c-4.j2c" )
		( "aaa/bbb/a1b-6c-4.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths, (sequenceParser::eDetectionNegative | sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles) );
	BOOST_CHECK_EQUAL( listSequence.size(), 4 );
}

BOOST_AUTO_TEST_CASE( MultiSequenceMinusSeparatorAmbiguityWithNegValues )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;
	std::vector<boost::filesystem::path> paths;
	boost::assign::push_back( paths )
		( "aaa/bbb/a1b2-1.j2c" )
		( "aaa/bbb/a1b2-3.j2c" )
		( "aaa/bbb/a1b2+5.j2c" )
		( "aaa/bbb/a1b2+10.j2c" )
		( "aaa/bbb/a1b24.j2c" )
		;

	listSequence = sequenceParser::sequenceFromFilenameList( paths, (sequenceParser::eDetectionNegative | sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles) );
	BOOST_CHECK_EQUAL( listSequence.size(), 1 );
	BOOST_CHECK_EQUAL( listSequence.front().getFirstTime(), -3 );
	BOOST_CHECK_EQUAL( listSequence.front().getLastTime(), 10 );
	BOOST_CHECK_EQUAL( listSequence.front().getFrameRanges().size(), 4 );
	BOOST_CHECK_EQUAL( listSequence.front().getNbFiles(), 4 );
}

BOOST_AUTO_TEST_CASE( MultiSequenceSingleFile )
{
	std::vector<boost::filesystem::path> paths;
	boost::assign::push_back( paths )
		( "aaa/bbb/a1b24.jpg" )
		;

	// Use last number
	boost::ptr_vector<sequenceParser::Sequence> listSequence_lastNumber =
			sequenceParser::sequenceFromFilenameList( paths, (sequenceParser::eDetectionNegative) );
	BOOST_CHECK_EQUAL( listSequence_lastNumber.size(), 1 );
	BOOST_CHECK_EQUAL( listSequence_lastNumber.front().getFirstTime(), 24 );
	BOOST_CHECK_EQUAL( listSequence_lastNumber.front().getLastTime(), 24 );
	BOOST_CHECK_EQUAL( listSequence_lastNumber.front().getNbFiles(), 1 );
    
	// Use first number
	boost::ptr_vector<sequenceParser::Sequence> listSequence_firstNumber =
			sequenceParser::sequenceFromFilenameList( paths, (sequenceParser::eDetectionNegative | sequenceParser::eDetectionSingleFileSeqUseFirstNumber) );
	BOOST_CHECK_EQUAL( listSequence_firstNumber.size(), 1 );
	BOOST_CHECK_EQUAL( listSequence_firstNumber.front().getFirstTime(), 1 );
	BOOST_CHECK_EQUAL( listSequence_firstNumber.front().getLastTime(), 1 );
	BOOST_CHECK_EQUAL( listSequence_firstNumber.front().getNbFiles(), 1 );
}


BOOST_AUTO_TEST_SUITE_END()
