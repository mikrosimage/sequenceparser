
%module sequenceParser

%include <std_vector.i>
%include <std_list.i>
%include <std_string.i>

%{
#include <Python.h>
#include <include/Detector.hpp>
#include <src/File.hpp>
#include <src/Folder.hpp>
#include <src/Sequence.hpp>
%}


namespace std {
%template(StringVector) vector<string>;
%template(StringList) list<string>;
}

%ignore fileInDir;
%ignore sequenceInDir;
%ignore folderInDir;
%ignore fileAndSequenceInDir;
%ignore fileObjectsInDir;
%include <include/Detector.hpp>




namespace sequenceParser {
%extend Detector
{
	// c++ code
	std::vector<std::string> fileInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::File> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->fileInDir ( p );
		for (std::list<boost::shared_ptr<sequenceParser::File> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::File> f = *it;
			filenames.push_back( f->getAbsoluteFilename().c_str() );
		}
		return filenames;
	}

	void printFileInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::File> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->fileInDir ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::File> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::File> f = *it;
			std::cout << *f << std::endl;
		}
	}

	std::vector<std::string> sequenceInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Sequence> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->sequenceInDir ( p );
		for (std::list<boost::shared_ptr<sequenceParser::Sequence> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Sequence> f = *it;
			filenames.push_back( f->getAbsoluteStandardPattern().c_str() );
		}
		return filenames;
	}

	void printSequenceInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Sequence> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->sequenceInDir ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::Sequence> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Sequence> f = *it;
			std::cout << *f << std::endl;
		}
	}

	std::vector<std::string> folderInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Folder> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->folderInDir ( p );
		for (std::list<boost::shared_ptr<sequenceParser::Folder> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Folder> f = *it;
			std::vector<boost::filesystem::path> pathDir = f->getFiles ();
			filenames.push_back( pathDir.at(0).c_str() );
		}
		return filenames;
	}

	void printFolderInDirectory( std::string path )
	{
		boost::filesystem::path p (path);
		std::list<boost::shared_ptr<sequenceParser::Folder> > outputFiles;
		std::vector<std::string> filenames;

		outputFiles = $self->folderInDir ( p, sequenceParser::eMaskOptionsColor );
		for (std::list<boost::shared_ptr<sequenceParser::Folder> >::iterator it = outputFiles.begin(); it != outputFiles.end(); it++)
		{
			boost::shared_ptr<sequenceParser::Folder> f = *it;
			std::cout << *f << std::endl;
		}
	}

}
}

