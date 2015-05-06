#include <detector.hpp>

#define BOOST_TEST_MODULE FileSequenceDetector

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

using boost::unit_test::test_suite;

void copyFile( std::string src, std::string dst )
{
	if( !boost::filesystem::exists( dst ) )
		boost::filesystem::copy_file( src, dst );
}

void createTmpDiretoriesFilesAndSequences()
{
	boost::filesystem::create_directories( "tmpTestSequence/root/film/" );
	boost::filesystem::create_directories( "tmpTestSequence/root/film/noStrictPadding" );
	boost::filesystem::create_directories( "tmpTestSequence/root/film/strictPadding" );
	boost::filesystem::create_directories( "tmpTestSequence/root/trash/" );
	boost::filesystem::create_directories( "tmpTestSequence/root/trash/dpx" );
	boost::filesystem::create_directories( "tmpTestSequence/root/trash/dpx/negative" );

	std::ofstream dummyFile;
	dummyFile.open( "tmpTestSequence/root/trash/img.jpg" );
	dummyFile << "This is a dummy Jpeg file.\n";
	dummyFile.close();

	dummyFile.open( "tmpTestSequence/root/trash/dpx/img.dpx" );
	dummyFile << "This is a dummy Dpx file.\n";
	dummyFile.close();

	// no strict padding: 0000, 0001, 0010, 0020, 0100
	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%04d", i );
		std::string outputFilename = "tmpTestSequence/root/film/strictPadding/img." + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}

	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%04d", i );
		std::string outputFilename = "tmpTestSequence/root/film/strictPadding/.img." + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}

	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%04d", i );
		std::string outputFilename = "tmpTestSequence/root/film/strictPadding/img." + std::string( numberName ) + ".jpg";
		copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
	}

	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%04d", i );
		std::string outputFilename = "tmpTestSequence/root/film/strictPadding/imgBroken." + std::string( numberName ) + ".jpg";
		copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
		if( i == 2 ) i = 9;
		if( i == 55 ) i = 99;
	}

	// no strict padding: 0, 1, 10, 20, 100
	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%d", i );
		std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/img." + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}

	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%d", i );
		std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/.img." + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}

	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%d", i );
		std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/img." + std::string( numberName ) + ".jpg";
		copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
	}

	for( int i = 0; i < 101; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%d", i );
		std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/imgBroken." + std::string( numberName ) + ".jpg";
		copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
		if( i == 2 ) i = 9;
		if( i == 55 ) i = 99;
	}
	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%04d", i );
		std::string outputFilename = "tmpTestSequence/root/trash/dpx/negative/img.-" + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}
	for( int i = 0; i < 100; i++ )
	{
		char numberName[20];
		sprintf( numberName, "%d", i );
		std::string outputFilename = "tmpTestSequence/root/trash/dpx/negative/seqTest.-" + std::string( numberName ) + ".dpx";
		copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
	}
}

void clearTmp()
{
	boost::filesystem::remove_all( "tmpTestSequence" );
}

bool testFindObjectInDiretory( const char* path, const size_t numberOfFolders, const size_t numberOfFiles, const size_t numberOfSequences, const size_t numberOfFileObjects )
{
	boost::ptr_vector<sequenceParser::FileObject> listFileObject;
	boost::ptr_vector<sequenceParser::Folder> listFolder;
	boost::ptr_vector<sequenceParser::File> listFile;
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	listFileObject = sequenceParser::fileObjectInDirectory( path, (sequenceParser::eTypeFolder | sequenceParser::eTypeFile | sequenceParser::eTypeSequence) );
	listFolder = sequenceParser::folderInDirectory( path );
	listFile = sequenceParser::fileInDirectory( path );
	listSequence = sequenceParser::sequenceInDirectory( path );

	BOOST_CHECK_EQUAL( listFileObject.size(), numberOfFileObjects );
	BOOST_CHECK_EQUAL( listFolder.size(), numberOfFolders );
	BOOST_CHECK_EQUAL( listFile.size(), numberOfFiles );
	BOOST_CHECK_EQUAL( listSequence.size(), numberOfSequences );

	if( listFileObject.size() != numberOfFileObjects ||
	    listFolder.size() != numberOfFolders ||
	    listFile.size() != numberOfFiles ||
	    listSequence.size() != numberOfSequences )
	{
		// BOOST_FOREACH( const sequenceParser::FileObject& f, listFileObject )
		// {
		// 	std::cout << "f: " << f << ", " << f.getType() << std::endl;
		// }
		// BOOST_FOREACH( const sequenceParser::Sequence& s, listSequence )
		// {
		// 	std::cout << "s: " << s << ", " << s.getType() << std::endl;
		// }
		std::cout << "test path: " << std::left << std::setw( 75 ) << path << std::right
			      << listFolder.size() << " -> " << numberOfFolders << " | "
			      << listFile.size() << " -> " << numberOfFiles << " | "
			      << listSequence.size() << " -> " << numberOfSequences << " | "
			      << listFileObject.size() << " -> " << numberOfFileObjects
			      << std::endl;
		return false;
	}
	return true;
//	boost::ptr_vector<sequenceParser::Sequence>::iterator it;
//	for(it=listSequence.begin() ; it != listSequence.end(); it++)
//	std::cout << (*it)->getAbsoluteStandardPattern() << std::endl;
}

bool testFindObjectInDiretory( const char* path, const sequenceParser::EDetection options, const size_t numberOfFolders, const size_t numberOfFiles, const size_t numberOfSequences, const size_t numberOfFileObjects )
{
	boost::ptr_vector<sequenceParser::FileObject> listFileObject;
	boost::ptr_vector<sequenceParser::Folder> listFolder;
	boost::ptr_vector<sequenceParser::File> listFile;
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	listFileObject = sequenceParser::fileObjectInDirectory( path, (sequenceParser::eTypeFolder | sequenceParser::eTypeFile | sequenceParser::eTypeSequence), options );
	listFolder = sequenceParser::folderInDirectory( path, options );
	listFile = sequenceParser::fileInDirectory( path, options );
	listSequence = sequenceParser::sequenceInDirectory( path, options );

	BOOST_CHECK_EQUAL( listFileObject.size(), numberOfFileObjects );
	BOOST_CHECK_EQUAL( listFolder.size(), numberOfFolders );
	BOOST_CHECK_EQUAL( listFile.size(), numberOfFiles );
	BOOST_CHECK_EQUAL( listSequence.size(), numberOfSequences );

	if( listFileObject.size() != numberOfFileObjects ||
		listFolder.size() != numberOfFolders ||
		listFile.size() != numberOfFiles ||
		listSequence.size() != numberOfSequences )
	{
		// BOOST_FOREACH( const sequenceParser::FileObject& f, listFileObject )
		// {
		// 	std::cout << "f: " << f << ", " << f.getType() << std::endl;
		// }
		// BOOST_FOREACH( const sequenceParser::Sequence& s, listSequence )
		// {
		// 	std::cout << "s: " << s << ", " << s.getType() << std::endl;
		// }
		std::cout << "test path [" << options << "]: "
			<< std::left << std::setw( 75 ) << path << std::right
			<< listFolder.size() << " -> " << numberOfFolders << " | "
			<< listFile.size() << " -> " << numberOfFiles << " | "
			<< listSequence.size() << " -> " << numberOfSequences << " | "
			<< listFileObject.size() << " -> " << numberOfFileObjects
			<< std::endl;
		return false;
	}
	
	return true;
//	boost::ptr_vector<sequenceParser::Sequence>::iterator it;
//	for(it=listSequence.begin() ; it != listSequence.end(); it++)
//	std::cout << (*it)->getAbsoluteStandardPattern() << std::endl;
}

bool testFirstSequenceLimits( const char* path, const int minValue, const int maxValue )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	listSequence = sequenceParser::sequenceInDirectory( path );

	BOOST_CHECK( !listSequence.empty() );
	if( listSequence.empty() )
		return false; // There is no sequence... so nothing to check

	sequenceParser::Sequence& seq = listSequence.front();

	if( seq.getFirstTime() != minValue || seq.getLastTime() != maxValue )
	{
		std::cout << "test sequence: " << path << " : set " << minValue << " -> " << maxValue
		          << " found " << seq.getFirstTime() << " -> " << seq.getLastTime()
		          << std::endl;
		return false;
	}

	BOOST_CHECK_EQUAL( seq.getFirstTime(), minValue );
	BOOST_CHECK_EQUAL( seq.getLastTime(), maxValue );
	return true;
}

bool testFirstSequenceLimits( const char* path, const sequenceParser::EDetection options, const int minValue, const int maxValue )
{
	boost::ptr_vector<sequenceParser::Sequence> listSequence;

	listSequence = sequenceParser::sequenceInDirectory( path, options );

	BOOST_CHECK( !listSequence.empty() );
	if( listSequence.empty() )
		return false; // There is no sequence... so nothing to check

	sequenceParser::Sequence& seq = listSequence.front();
	if( seq.getFirstTime() != minValue || seq.getLastTime() != maxValue )
	{
		std::cout << "test sequence: " << path << " : set = " << minValue << " -> " << maxValue << " found "
		          << seq.getFirstTime() << " -> " << seq.getLastTime()
		          << std::endl;
		return false;
	}
	BOOST_CHECK_EQUAL( seq.getFirstTime(), minValue );
	BOOST_CHECK_EQUAL( seq.getLastTime(), maxValue );
	return true;
}

BOOST_AUTO_TEST_CASE( TestSequence )
{
	createTmpDiretoriesFilesAndSequences();

	std::cout << std::setw( 85 ) << " " << " Folder |  File  | Sequence | FileObjects" << std::endl;

    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence"                                                                                                    , 1, 0, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root"                                                                                               , 2, 0, 0, 2 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/"                                                                                        , 1, 1, 0, 2 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/"                                                                                    , 1, 1, 0, 2 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/"                                                                           , 0, 0, 2, 2 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.####.dpx"                                                               , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.#####.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.-####.dpx"                                                              , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFirstSequenceLimits ( "tmpTestSequence/root/trash/dpx/negative/img.-####.dpx"                                                              , 0, 99 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/seqTest.@.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/seqTest.-@.dpx"                                                             , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFirstSequenceLimits ( "tmpTestSequence/root/trash/dpx/negative/seqTest.-@.dpx"                                                             , 0, 99 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/"                      , sequenceParser::eDetectionNegative | sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles , 0, 0, 2, 3 ) ); // should be (0, 0, 2, 2) // ERROR -0 is not detected in the sequence
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/seqTest.@.dpx"         , sequenceParser::eDetectionNegative        , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFirstSequenceLimits ( "tmpTestSequence/root/trash/dpx/negative/seqTest.@.dpx"         , sequenceParser::eDetectionNegative        , -99, -1 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/seqTest.-@.dpx"        , sequenceParser::eDetectionNegative        , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFirstSequenceLimits ( "tmpTestSequence/root/trash/dpx/negative/seqTest.-@.dpx"        , sequenceParser::eDetectionNegative        , -99, -1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.#### .dpx"         , sequenceParser::eDetectionNegative        , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.####.dpx"          , sequenceParser::eDetectionNegative        , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.#####.dpx"         , sequenceParser::eDetectionNegative        , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/trash/dpx/negative/img.-####.dpx"         , sequenceParser::eDetectionNegative        , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFirstSequenceLimits ( "tmpTestSequence/root/trash/dpx/negative/img.-####.dpx"         , sequenceParser::eDetectionNegative        , -99, 0 ) );

    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/"                                                                           , 0, 0, 3, 3 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/"                      , sequenceParser::eDetectionNone                , 0, 0, 4, 4 ) );

    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.####.dpx"                                                               , 0, 0, 1, 1 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"                                                               , 0, 1, 0, 1 ) ); // don't work, but should detect the sequence instead
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.0050.dpx"                                                                 , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*0050.dpx"                                                                  , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.0050.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/????0050.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.####.dpx"                                                                 , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.@.dpx"                                                                    , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.####.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.@.dpx"                                                                  , 0, 0, 1, 1 ) );

    boost::filesystem::current_path("tmpTestSequence/root/film/strictPadding/");
//    BOOST_CHECK( testFindObjectInDiretory( "./img.0050.dpx"                                                                                                     , 0, 1, 0, 1 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "img.0050.dpx"                                                                                                       , 0, 1, 0, 1 ) );
    boost::filesystem::current_path("../../../../");

//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"          , sequenceParser::eDetectionNone, 0, 1, 0, 1 ) );
	// no eDetectionSequenceBasedOnASingleFile, so it should not detect the sequence:
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.0050.dpx"            , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles | sequenceParser::eDetectionIgnoreDotFile, 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*0050.dpx"             , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles | sequenceParser::eDetectionIgnoreDotFile, 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.0050.dpx"          , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles, 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/????0050.dpx"          , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles, 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.####.dpx"            , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles | sequenceParser::eDetectionIgnoreDotFile, 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/*.@.dpx"               , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles | sequenceParser::eDetectionIgnoreDotFile, 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.####.dpx"          , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles, 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/???.@.dpx"             , sequenceParser::eDetectionSequenceNeedAtLeastTwoFiles, 0, 0, 1, 1 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"          , sequenceParser::eDetectionNone                , 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.50.dpx"                                                                 , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.50.dpx"            , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.0500.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.0500.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.000050.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.000050.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.####.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.####.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.###.dpx"                                                                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.###.dpx"           , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.#####.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.#####.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%04d.dpx"                                                               , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%04d.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%03d.dpx"                                                               , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%03d.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%05d.dpx"                                                               , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.%05d.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.@.dpx"                                                                  , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/img.@.dpx"             , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );

//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.0050.dpx"                                                              , 0, 0, 0, 0 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.0050.dpx"         , sequenceParser::eDetectionNone                , 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.50.dpx"                                                                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.50.dpx"           , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.0500.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.0500.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.000050.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.000050.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.####.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.####.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.###.dpx"                                                               , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.###.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.#####.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.#####.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%04d.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%04d.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%03d.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%03d.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%05d.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.%05d.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.@.dpx"                                                                 , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/strictPadding/.img.@.dpx"            , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );


    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/"                                                                         , 0, 0, 3, 3 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/"                    , sequenceParser::eDetectionNone                , 0, 0, 4, 4 ) );

    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.50.dpx"                                                               , 0, 1, 0, 1 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.50.dpx"          , sequenceParser::eDetectionNone                , 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0500.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0500.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.####.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.####.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.###.dpx"                                                              , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.###.dpx"         , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.#####.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.#####.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%04d.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%04d.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%05d.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%05d.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%03d.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.%03d.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.@.dpx"                                                                , 0, 0, 1, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/img.@.dpx"           , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );

    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.50.dpx"                                                              , 0, 0, 0, 0 ) );
//    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.50.dpx"         , sequenceParser::eDetectionNone                , 0, 1, 0, 1 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0500.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0500.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.####.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.####.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.###.dpx"                                                             , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.###.dpx"        , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.#####.dpx"                                                           , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.#####.dpx"      , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%04d.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%04d.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%05d.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%05d.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%03d.dpx"                                                            , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.%03d.dpx"       , sequenceParser::eDetectionNone                , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.@.dpx"                                                               , 0, 0, 0, 0 ) );
    BOOST_CHECK( testFindObjectInDiretory( "tmpTestSequence/root/film/noStrictPadding/.img.@.dpx"          , sequenceParser::eDetectionNone                , 0, 0, 1, 1 ) );

	//clearTmp();
}
