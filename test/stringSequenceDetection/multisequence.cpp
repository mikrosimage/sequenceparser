#include <Detector.hpp>

#include <boost/assign/std/vector.hpp>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( MutiSequenceDetection )

BOOST_AUTO_TEST_CASE( SimpleMultiSequence )
{
	sequenceParser::Detector detector;
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

	listSequence = detector.sequenceFromFilenameList( paths );

	std::cout << "listSequence.size()" << listSequence.size() << std::endl;
	BOOST_CHECK( listSequence.size() == 3 );
}

BOOST_AUTO_TEST_CASE( SimpleMultiSequenceMultiLevel )
{
	sequenceParser::Detector detector;
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

	listSequence = detector.sequenceFromFilenameList( paths );

//	std::cout << "AA: " << listSequence.size() << std::endl;
	BOOST_CHECK( listSequence.size() == 2 );
	
	std::cout << listSequence.front().getNbFiles() << std::endl;
	BOOST_CHECK( listSequence.front().getNbFiles() == 3 );
	
	std::cout << listSequence.back().getNbFiles() << std::endl;
	BOOST_CHECK( listSequence.back().getNbFiles() == 4 );
}

BOOST_AUTO_TEST_CASE( MultiSequenceMultiLevelMultiPadding )
{
	sequenceParser::Detector detector;
	
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

		listSequence = detector.sequenceFromFilenameList( paths );
		//std::cout << "listSequence.size(): " << listSequence.size() << std::endl;
		BOOST_CHECK( listSequence.size() == 2 );
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

		listSequence = detector.sequenceFromFilenameList( paths );
		std::cout << "listSequence.size(): " << listSequence.size() << std::endl;
		BOOST_CHECK( listSequence.size() == 3 );
	}
}

BOOST_AUTO_TEST_CASE( MultiSequenceMultiLevelMultiPaddingWithNegValues )
{
	using namespace sequenceParser;
	Detector detector;
	
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

	listSequence = detector.sequenceFromFilenameList( paths, (eMaskOptionsDotFile | eMaskOptionsNegativeIndexes) );
	std::cout << "listSequence.size(): " << listSequence.size() << std::endl;
	BOOST_CHECK( listSequence.size() == 4 );
}


BOOST_AUTO_TEST_SUITE_END()
