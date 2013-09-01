#ifndef _SEQUENCE_PARSER_DETECTOR_HPP_
#define _SEQUENCE_PARSER_DETECTOR_HPP_

#include "commonDefinitions.hpp"
#include "FileObject.hpp"
#include "Sequence.hpp"
#include "File.hpp"
#include "Folder.hpp"

#include <boost/ptr_container/ptr_vector.hpp>

#include <iostream>
#include <iomanip>

namespace sequenceParser {

boost::ptr_vector<File> fileInDirectory( const std::string& directory,
													 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<File> fileInDirectory( const std::string& directory,
													 std::vector<std::string>& filters,
													 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<Sequence> sequenceInDirectory( const std::string& directory,
															 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<Sequence> sequenceInDirectory( const std::string& directory,
															 std::vector<std::string>& filters,
															 const EMaskOptions desc = eMaskOptionsDefault );


boost::ptr_vector<Sequence> sequenceFromFilenameList( const std::vector<boost::filesystem::path>& filenames, const EMaskOptions desc = eMaskOptionsNone );


boost::ptr_vector<FileObject> fileAndSequenceInDirectory( const std::string& directory,
																	  const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<FileObject> fileAndSequenceInDirectory( const std::string& directory,
																	  std::vector<std::string>& filters,
																	  const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<Folder> folderInDirectory( const std::string& directory,
														 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<Folder> folderInDirectory( const std::string& directory,
														 std::vector<std::string>& filters,
														 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<FileObject> fileObjectInDirectory( const std::string& directory,
																 const EMaskType mask = eMaskTypeDefault,
																 const EMaskOptions desc = eMaskOptionsDefault );

boost::ptr_vector<FileObject> fileObjectInDirectory( const std::string& directory,
																 std::vector<std::string>& filters,
																 const EMaskType mask = eMaskTypeDefault,
																 const EMaskOptions desc = eMaskOptionsDefault );


}

#endif
