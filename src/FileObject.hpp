#ifndef _SEQUENCE_PARSER_FILEOBJECT_HPP_
#define _SEQUENCE_PARSER_FILEOBJECT_HPP_

#include <boost/filesystem.hpp>
#include <commonDefinitions.hpp>


namespace sequenceParser {

/**
 * @brief A container for files, directories and sequences.
 */
class FileObject
{
public:

	FileObject()
	{
		_directory.clear();
		_type = eMaskTypeUndefined;
		_options = eMaskOptionsNone;
		setColorActive( _options & eMaskOptionsColor );
	}

	FileObject( const EMaskOptions options )
	{
		_directory.clear();
		_type = eMaskTypeUndefined;
		_options = options;
		setColorActive( _options & eMaskOptionsColor );
	}

	/**
	 * @brief Construct a FileObject with given informations.
	 */
	FileObject( const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options )
	{
		init( directory, type, options );
	}

	virtual ~FileObject();

#ifndef SWIG
	friend std::ostream& operator<<( std::ostream& os, const FileObject& fo );
#endif

	virtual std::ostream& getCout( std::ostream& os ) const = 0;

	/**
	 * @todo: can we remove this?
	 */
	virtual std::vector<boost::filesystem::path> getFiles() const = 0;

	/// @todo
	// virtual std::string getName() const = 0;
	//  boost::filesystem::path getAbsoluteName() const
	//  {
	//      return getDirectory() / getName();
	//  }

	inline boost::filesystem::path getDirectory() const
	{
		return _directory;
	}

	inline boost::filesystem::path getAbsoluteDirectory() const
	{
		return boost::filesystem::absolute( _directory );
	}

	inline void setDirectory( const boost::filesystem::path& p )
	{
		_directory = p;
	}

	void setDirectoryFromPath( const boost::filesystem::path& p );

	EMaskOptions getMaskOptions() const
	{
		return _options;
	}

	EMaskType getMaskType() const
	{
		return _type;
	}

	virtual inline void clear()
	{
		_directory.clear();
		_type = eMaskTypeDefault;
		_options = eMaskOptionsDefault;
	}

private:

	void init( const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options )
	{
		_directory = directory;
		_type = type;
		_options = options;
		setColorActive( _options & eMaskOptionsColor );
	}

	void setColorActive( bool activate = false )
	{
		if( activate )
		{
			_kColorStd = kColorStd;
			_kColorFolder = kColorFolder;
			_kColorFile = kColorFile;
			_kColorSequence = kColorSequence;
			_kColorError = kColorError;
		}
		else
		{
			_kColorStd = "";
			_kColorFolder = "";
			_kColorFile = "";
			_kColorSequence = "";
			_kColorError = "";
		}
	}

protected:

	inline bool showProperties() const
	{
		return _options & eMaskOptionsProperties;
	}

	inline bool showRelativePath() const
	{
		return _options & eMaskOptionsPath;
	}

	inline bool showAbsolutePath() const
	{
		return _options & eMaskOptionsAbsolutePath;
	}

protected:
	boost::filesystem::path _directory; ///< directory
	EMaskType _type; ///< specify type of object
	EMaskOptions _options; ///< specify output options of object, common for each objects
	std::string _kColorStd;
	std::string _kColorFolder;
	std::string _kColorFile;
	std::string _kColorSequence;
	std::string _kColorError;
};

}

#endif


