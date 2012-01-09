#include <Detector.hpp>

#define BOOST_TEST_MODULE SequenceDetector

#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using boost::unit_test::test_suite;

void copyFile( std::string src, std::string dst )
{
	if ( ! boost::filesystem::exists( dst ) )
		boost::filesystem::copy_file(src, dst);
}

void createTmpDiretoriesFilesAndSequences()
{
	boost::filesystem::create_directories( "tmpTestSequence/root/film/" );
	boost::filesystem::create_directories( "tmpTestSequence/root/trash/" );
	boost::filesystem::create_directories( "tmpTestSequence/root/trash/dpx" );

	std::ofstream dummyFile;
	dummyFile.open ("tmpTestSequence/root/trash/img.jpg");
	dummyFile << "This is a dummy Jpeg file.\n";
	dummyFile.close();

	dummyFile.open ("tmpTestSequence/root/trash/dpx/img.dpx");
	dummyFile << "This is a dummy Dpx file.\n";
	dummyFile.close();

	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0000.dpx");
	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0001.dpx");
	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0002.dpx");

	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0000.dpx");
	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0001.dpx");
	copyFile("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0002.dpx");

	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0000.jpg");
	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0001.jpg");
	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0002.jpg");

	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0000.jpg");
	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0001.jpg");
	copyFile("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0010.jpg");
}

void clearTmp()
{
	boost::filesystem::remove_all("tmpTestSequence");
}

void testFindObjectInDiretory ( const char* path, const size_t numberOfFolders, const size_t numberOfFiles, const size_t numberOfSequences, const size_t numberOfFileObjects )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::FileObject > > listFileObject;
	std::list<boost::shared_ptr<sequenceParser::Folder     > > listFolder;
	std::list<boost::shared_ptr<sequenceParser::File       > > listFile;
	std::list<boost::shared_ptr<sequenceParser::Sequence   > > listSequence;

	listFileObject = detector.fileObjectInDirectory  ( path, (sequenceParser::EMaskType) ( sequenceParser::eMaskTypeDirectory | sequenceParser::eMaskTypeFile | sequenceParser::eMaskTypeSequence ) );
	listFolder     = detector.folderInDirectory      ( path );
	listFile       = detector.fileInDirectory        ( path );
	listSequence   = detector.sequenceInDirectory    ( path );

	std::cout << "test path: " << std::left << std::setw(50) << path << std::right << std::setw(8) << listFolder.size() << " | " << std::setw(8) << listFile.size() << " | " << std::setw(8) << listSequence.size() << " | " << std::setw(11) << listFileObject.size() << std::endl;

	BOOST_CHECK( listFileObject.size() == numberOfFileObjects );
	BOOST_CHECK( listFolder.size()     == numberOfFolders     );
	BOOST_CHECK( listFile.size()       == numberOfFiles       );
	BOOST_CHECK( listSequence.size()   == numberOfSequences   );
}

void testFindObjectInDiretory ( const char* path, const sequenceParser::EMaskOptions options, const size_t numberOfFolders, const size_t numberOfFiles, const size_t numberOfSequences, const size_t numberOfFileObjects )
{
	sequenceParser::Detector detector;
	std::list<boost::shared_ptr<sequenceParser::FileObject > > listFileObject;
	std::list<boost::shared_ptr<sequenceParser::Folder     > > listFolder;
	std::list<boost::shared_ptr<sequenceParser::File       > > listFile;
	std::list<boost::shared_ptr<sequenceParser::Sequence   > > listSequence;

	listFileObject = detector.fileObjectInDirectory  ( path, (sequenceParser::EMaskType) ( sequenceParser::eMaskTypeDirectory | sequenceParser::eMaskTypeFile | sequenceParser::eMaskTypeSequence ) ,  options );
	listFolder     = detector.folderInDirectory      ( path, options );
	listFile       = detector.fileInDirectory        ( path, options );
	listSequence   = detector.sequenceInDirectory    ( path, options );

	std::cout << "test path: " << std::left << std::setw(50) << path << std::right << std::setw(8) << listFolder.size() << " | " << std::setw(8) << listFile.size() << " | " << std::setw(8) << listSequence.size() << " | " << std::setw(11) << listFileObject.size() << std::endl;

	BOOST_CHECK( listFileObject.size() == numberOfFileObjects );
	BOOST_CHECK( listFolder.size()     == numberOfFolders     );
	BOOST_CHECK( listFile.size()       == numberOfFiles       );
	BOOST_CHECK( listSequence.size()   == numberOfSequences   );
}

BOOST_AUTO_TEST_CASE(TestSequence)
{
	createTmpDiretoriesFilesAndSequences();

	std::cout << std::setw(61) << " " << " Folder  |   File   | Sequence | FileObjects" << std::endl;

	testFindObjectInDiretory ( "tmpTestSequence"                                                                    , 1, 0, 0, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root"                                                               , 2, 0, 0, 2 );
	testFindObjectInDiretory ( "tmpTestSequence/root/trash/"                                                        , 1, 1, 0, 2 );
	testFindObjectInDiretory ( "tmpTestSequence/root/trash/dpx/"                                                    , 0, 1, 0, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root/film/"                                                         , 0, 0, 3, 3 );

	testFindObjectInDiretory ( "tmpTestSequence/root/film/img.####.dpx"                                             , 0, 0, 1, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root/film/img.@.dpx"                                                , 0, 0, 1, 1 );

	testFindObjectInDiretory ( "tmpTestSequence/root/film/img.####.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root/film/img.@.dpx"           , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );

	testFindObjectInDiretory ( "tmpTestSequence/root/film/.img.####.dpx"                                            , 0, 0, 0, 0 );
	testFindObjectInDiretory ( "tmpTestSequence/root/film/.img.####.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );

	testFindObjectInDiretory ( "tmpTestSequence/root/film/"                    , sequenceParser::eMaskOptionsDotFile, 0, 0, 4, 4 );

	clearTmp();
}

