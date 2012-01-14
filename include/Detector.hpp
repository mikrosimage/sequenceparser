#ifndef _SEQUENCE_PARSER_DETECTOR_HPP_
#define _SEQUENCE_PARSER_DETECTOR_HPP_

#include <FileObject.hpp>
#include <FileStrings.hpp>
#include <FileNumbers.hpp>
#include <Sequence.hpp>
#include <File.hpp>
#include <Folder.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>

namespace sequenceParser {

class Detector
{
public :
    Detector();
    ~Detector();

    /**
     ** fileInDirectory methods
     **
     **/
    std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory                 ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::File> >       fileInDirectory                 ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileInDirectory            ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileInDirectory            ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );


    std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory             ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::Sequence> >   sequenceInDirectory             ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printSequenceInDirectory        ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printSequenceInDirectory        ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory      ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::FileObject> > fileAndSequenceInDirectory      ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileAndSequenceInDirectory ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileAndSequenceInDirectory ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory               ( const std::string& directory,
                                                                                               const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::Folder> >     folderInDirectory               ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFolderInDirectory          ( const std::string& directory,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFolderInDirectory          ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory           ( const std::string& directory,
                                                                                                const sequenceParser::EMaskType mask = eMaskTypeDefault,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    std::list<boost::shared_ptr<sequenceParser::FileObject> > fileObjectInDirectory           ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskType mask = eMaskTypeDefault,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileObjectInDirectory      ( const std::string& directory,
                                                                                                const sequenceParser::EMaskType mask = eMaskTypeDefault,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

    void                                                      printFileObjectInDirectory      ( const std::string& directory,
                                                                                                std::vector<std::string>& filters,
                                                                                                const sequenceParser::EMaskType mask = eMaskTypeDefault,
                                                                                                const sequenceParser::EMaskOptions desc = eMaskOptionsDefault );

private:

    /**
     * Detect if the researchPath is a correct pattern ( a folder, a file or a pattern with an valid parent directory )
     *
     * @param[inout] reseachPath path/pattern to be checked, return the valid research directory
     * @param[inout] filters to add pattern of detection present in the researchPath
     *
     * @return return false if the research path is incorrect with a folder, a file or a pattern with an valid parent directory
     */
    bool detectDirectoryInResearch ( std::string& researchPath, std::vector<std::string>& filters );

    /**
     * Detect if the filename is filtered by one of the filter
     *
     * @param[in] filename filename need to be check if it filtered
     * @param[in] filters vector of filters
     * @param[in] desc enable research options (Cf. EMaskOptions in commonDefinitions.hpp )
     *
     * @return return true if the filename is filtered by filter(s)
     */
    bool isNotFilter( std::string filename, std::vector<std::string>& filters, const EMaskOptions desc );

    /**
     * @brief Create a sequence
     * @param[in] id: the sequence identifier
     * @param[inout] nums: the list of sequence of number inside each filename
     * @warning We modify nums in place, because we don't use it after
     *          so there is no reason to create a copy.
     * @return a sequence object with all informations
     */
    std::list<sequenceParser::Sequence> buildSequence( const boost::filesystem::path& directory, const sequenceParser::FileStrings& id, std::list<sequenceParser::FileNumbers>& nums, const sequenceParser::EMaskOptions& desc );

    /**
     * @brief Construct id and nums from a filename.
     * @param[in] str: the string to process (filename)
     * @param[out] id: list of strings
     * @param[out] nums: list of integers
     * @return number of decteted numbers
     */
    std::size_t seqConstruct( const std::string& str, FileStrings& id, FileNumbers& nums );
};

}
#endif
