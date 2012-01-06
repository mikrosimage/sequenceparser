#include <Detector.hpp>

#define BOOST_TEST_MODULE SequenceDetector

#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using boost::unit_test::test_suite;

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

	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0000.dpx");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0001.dpx");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/img.0002.dpx");

	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0000.dpx");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0001.dpx");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/dpx/img.dpx", "tmpTestSequence/root/film/.img.0002.dpx");

	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0000.jpg");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0001.jpg");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/img.0002.jpg");

	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0000.jpg");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0001.jpg");
	boost::filesystem::copy_file("tmpTestSequence/root/trash/img.jpg", "tmpTestSequence/root/film/imgBroken.0010.jpg");
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

	listFileObject = detector.fileObjectsInDir ( path, (sequenceParser::EMaskType) ( sequenceParser::eMaskTypeDirectory | sequenceParser::eMaskTypeFile | sequenceParser::eMaskTypeSequence ) );
	listFolder     = detector.folderInDir  ( path );
	listFile       = detector.fileInDir    ( path );
	listSequence   = detector.sequenceInDir( path );

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

	listFileObject = detector.fileObjectsInDir ( path, (sequenceParser::EMaskType) ( sequenceParser::eMaskTypeDirectory | sequenceParser::eMaskTypeFile | sequenceParser::eMaskTypeSequence ) ,  options );
	listFolder     = detector.folderInDir      ( path, options );
	listFile       = detector.fileInDir        ( path, options );
	listSequence   = detector.sequenceInDir    ( path, options );

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

	testFindObjectInDiretory ( "tmpTestSequence"                  , 1, 0, 0, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root"             , 2, 0, 0, 2 );
	testFindObjectInDiretory ( "tmpTestSequence/root/trash/"      , 1, 1, 0, 2 );
	testFindObjectInDiretory ( "tmpTestSequence/root/trash/dpx/"  , 0, 1, 0, 1 );
	testFindObjectInDiretory ( "tmpTestSequence/root/film/"       , 0, 0, 3, 3 );

	testFindObjectInDiretory ( "tmpTestSequence/root/film/"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 4, 4 );

	clearTmp();
}

