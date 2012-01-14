#include <Detector.hpp>

#define BOOST_TEST_MODULE SequenceDetector

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
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
    boost::filesystem::create_directories( "tmpTestSequence/root/film/noStrictPadding" );
    boost::filesystem::create_directories( "tmpTestSequence/root/film/strictPadding" );
    boost::filesystem::create_directories( "tmpTestSequence/root/trash/" );
    boost::filesystem::create_directories( "tmpTestSequence/root/trash/dpx" );

    std::ofstream dummyFile;
    dummyFile.open ("tmpTestSequence/root/trash/img.jpg");
    dummyFile << "This is a dummy Jpeg file.\n";
    dummyFile.close();

    dummyFile.open ("tmpTestSequence/root/trash/dpx/img.dpx");
    dummyFile << "This is a dummy Dpx file.\n";
    dummyFile.close();

    // no strict padding: 0000, 0001, 0010, 0020, 0100
    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%04d", i );
        std::string outputFilename = "tmpTestSequence/root/film/strictPadding/img."+std::string(numberName)+".dpx";
        copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
    }

    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%04d", i );
        std::string outputFilename = "tmpTestSequence/root/film/strictPadding/.img."+std::string(numberName)+".dpx";
        copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
    }

    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%04d", i );
        std::string outputFilename = "tmpTestSequence/root/film/strictPadding/img."+std::string(numberName)+".jpg";
        copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
    }

    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%04d", i );
        std::string outputFilename = "tmpTestSequence/root/film/strictPadding/imgBroken."+std::string(numberName)+".jpg";
        copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
        if( i == 2 ) i = 9;
        if( i == 55 ) i = 99;
    }

    // no strict padding: 0, 1, 10, 20, 100
    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%d", i );
        std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/img."+std::string(numberName)+".dpx";
        copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
    }

    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%d", i );
        std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/.img."+std::string(numberName)+".dpx";
        copyFile( "tmpTestSequence/root/trash/dpx/img.dpx", outputFilename );
    }

    for( int i =0; i< 100; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%d", i );
        std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/img."+std::string(numberName)+".jpg";
        copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
    }

    for( int i =0; i< 101; i++ )
    {
        char numberName[20];
        sprintf( numberName,"%d", i );
        std::string outputFilename = "tmpTestSequence/root/film/noStrictPadding/imgBroken."+std::string(numberName)+".jpg";
        copyFile( "tmpTestSequence/root/trash/img.jpg", outputFilename );
        if( i == 2 ) i = 9;
        if( i == 55 ) i = 99;
    }
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

    std::cout << "test path: " << std::left << std::setw(60) << path << std::right << std::setw(8) << listFolder.size() << " | " << std::setw(8) << listFile.size() << " | " << std::setw(8) << listSequence.size() << " | " << std::setw(11) << listFileObject.size() << std::endl;

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

    std::cout << "test path: " << std::left << std::setw(60) << path << std::right << std::setw(8) << listFolder.size() << " | " << std::setw(8) << listFile.size() << " | " << std::setw(8) << listSequence.size() << " | " << std::setw(11) << listFileObject.size() << std::endl;

    BOOST_CHECK( listFileObject.size() == numberOfFileObjects );
    BOOST_CHECK( listFolder.size()     == numberOfFolders     );
    BOOST_CHECK( listFile.size()       == numberOfFiles       );
    BOOST_CHECK( listSequence.size()   == numberOfSequences   );
}

BOOST_AUTO_TEST_CASE(TestSequence)
{
    createTmpDiretoriesFilesAndSequences();

    std::cout << std::setw(71) << " " << " Folder  |   File   | Sequence | FileObjects" << std::endl;

    testFindObjectInDiretory ( "tmpTestSequence"                                                                                    , 1, 0, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root"                                                                               , 2, 0, 0, 2 );
    testFindObjectInDiretory ( "tmpTestSequence/root/trash/"                                                                        , 1, 1, 0, 2 );
    testFindObjectInDiretory ( "tmpTestSequence/root/trash/dpx/"                                                                    , 0, 1, 0, 1 );

    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/"                                                           , 0, 0, 3, 3 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/"                      , sequenceParser::eMaskOptionsDotFile, 0, 0, 4, 4 );

    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"                                               , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.0050.dpx"                                                 , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*0050.dpx"                                                  , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.0050.dpx"                                               , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/????0050.dpx"                                               , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.####.dpx"                                                 , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.@.dpx"                                                    , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.####.dpx"                                               , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.@.dpx"                                                  , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"          , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.0050.dpx"            , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*0050.dpx"             , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.0050.dpx"          , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/????0050.dpx"          , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.####.dpx"            , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/*.@.dpx"               , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.####.dpx"          , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/???.@.dpx"             , sequenceParser::eMaskOptionsSequenceBasedOnFilename, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.0050.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.50.dpx"                                                 , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.50.dpx"            , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.0500.dpx"                                               , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.0500.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.000050.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.000050.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.####.dpx"                                               , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.####.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.###.dpx"                                                , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.###.dpx"           , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.#####.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.#####.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%04d.dpx"                                               , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%04d.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%03d.dpx"                                               , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%03d.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%05d.dpx"                                               , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.%05d.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.@.dpx"                                                  , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/img.@.dpx"             , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );

    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.0050.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.0050.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.50.dpx"                                                , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.50.dpx"           , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.0500.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.0500.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.000050.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.000050.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.####.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.####.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.###.dpx"                                               , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.###.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.#####.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.#####.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%04d.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%04d.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%03d.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%03d.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%05d.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.%05d.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.@.dpx"                                                 , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/strictPadding/.img.@.dpx"            , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );


    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/"                                                         , 0, 0, 3, 3 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/"                    , sequenceParser::eMaskOptionsDotFile, 0, 0, 4, 4 );

    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.50.dpx"                                               , 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.50.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0500.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0500.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.0050.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.####.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.####.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.###.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.###.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.#####.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.#####.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%04d.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%04d.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%05d.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%05d.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%03d.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.%03d.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.@.dpx"                                                , 0, 0, 1, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/img.@.dpx"           , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );

    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.50.dpx"                                              , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.50.dpx"         , sequenceParser::eMaskOptionsDotFile, 0, 1, 0, 1 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0500.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0500.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.0050.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.####.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.####.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.###.dpx"                                             , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.###.dpx"        , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.#####.dpx"                                           , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.#####.dpx"      , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%04d.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%04d.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%05d.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%05d.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%03d.dpx"                                            , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.%03d.dpx"       , sequenceParser::eMaskOptionsDotFile, 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.@.dpx"                                               , 0, 0, 0, 0 );
    testFindObjectInDiretory ( "tmpTestSequence/root/film/noStrictPadding/.img.@.dpx"          , sequenceParser::eMaskOptionsDotFile, 0, 0, 1, 1 );

    //clearTmp();
}

