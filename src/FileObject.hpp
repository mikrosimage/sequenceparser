#ifndef _SEQUENCE_PARSER_FILEOBJECT_HPP_
#define _SEQUENCE_PARSER_FILEOBJECT_HPP_

#include <boost/filesystem.hpp>
#include <commonDefinitions.hpp>


namespace sequenceParser {

/**
 * @brief A container for files, directories and sequences.
 */
class FileObject : private boost::noncopyable
{
public:

	FileObject()
	{
		_directory.clear();
		_type = eTypeUndefined;
		_displayOptions = eDisplayNone;
		setColorActive( _displayOptions & eDisplayColor );
	}

	FileObject( const EDisplay displayOptions )
	{
		_directory.clear();
		_type = eTypeUndefined;
		_displayOptions = displayOptions;
		setColorActive( _displayOptions & eDisplayColor );
	}

	/**
	 * @brief Construct a FileObject with given informations.
	 */
	FileObject( const boost::filesystem::path& directory, const EType& type, const EDisplay& displayOptions )
	{
		init( directory, type, displayOptions );
	}

	FileObject( const FileObject& other )
	{
		operator=( other );
	}
	
	FileObject& operator=( const FileObject& other )
	{
		_directory = other._directory;
		_type = other._type;
		_displayOptions = other._displayOptions;
		_kColorStd = other._kColorStd;
		_kColorFolder = other._kColorFolder;
		_kColorFile = other._kColorFile;
		_kColorSequence = other._kColorSequence;
		_kColorError = other._kColorError;
		return *this;
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

	EDisplay getDisplayOptions() const
	{
		return _displayOptions;
	}

	EType getType() const
	{
		return _type;
	}

	virtual inline void clear()
	{
		_directory.clear();
		_type = eTypeUndefined;
		_displayOptions = eDisplayDefault;
	}
	
	virtual FileObject* clone() const = 0;
	
private:

	void init( const boost::filesystem::path& directory, const EType& type, const EDisplay& displayOptions )
	{
		_directory = directory;
		_type = type;
		_displayOptions = displayOptions;
		setColorActive( _displayOptions & eDisplayColor );
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
		return _displayOptions & eDisplayProperties;
	}

	inline bool showRelativePath() const
	{
		return _displayOptions & eDisplayPath;
	}

	inline bool showAbsolutePath() const
	{
		return _displayOptions & eDisplayAbsolutePath;
	}

protected:
	boost::filesystem::path _directory; ///< directory
	EType _type; ///< specify type of object
	
	///@todo Remove this from here!
	///@{
	EDisplay _displayOptions; ///< specify output options of object, common for each objects
	std::string _kColorStd;
	std::string _kColorFolder;
	std::string _kColorFile;
	std::string _kColorSequence;
	std::string _kColorError;
	///@}
};

#ifndef SWIG
inline FileObject* new_clone( const FileObject& a )
{
	return a.clone();
}
#endif


}

#endif


