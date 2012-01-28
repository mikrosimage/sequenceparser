
inline bool Sequence::init( const Time first, const Time last, const Time step, const EPattern accept )
{
	return this->init( _directory.filename().string(), first, last, step, accept );
}

inline bool Sequence::initFromPattern( const boost::filesystem::path& dir, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options, const EPattern accept )
{
	return init( pattern, firstTime, lastTime, step, accept );
}

inline bool Sequence::initFromDetection( const EPattern& accept )
{
	return this->initFromDetection( getDirectory().string(), accept );
}

inline std::string Sequence::getAbsoluteFilenameAt( const Time time ) const
{
	return ( getAbsoluteDirectory() / getFilenameAt( time ) ).string();
}

inline std::string Sequence::getFirstFilename() const
{
	return getFilenameAt( getFirstTime() );
}

inline std::string Sequence::getFilenameAt( const Time time ) const
{
	std::ostringstream o;
	if( time >= 0 )
	{
		// "prefix.0001.jpg"
		o << _prefix << std::setw( _padding ) << std::setfill( _fillCar ) << time << _suffix;
	}
	else
	{
		// "prefix.-0001.jpg" (and not "prefix.000-1.jpg")
		o << _prefix << "-" << std::setw( _padding ) << std::setfill( _fillCar ) << std::abs( (int) time ) << _suffix;
	}
	return o.str();
}

inline std::string Sequence::getAbsoluteFirstFilename() const
{
	return ( getAbsoluteDirectory() / getFilenameAt( getFirstTime() ) ).string();
}

inline std::string Sequence::getAbsoluteLastFilename() const
{
	return ( getAbsoluteDirectory() / getFilenameAt( getLastTime() ) ).string();
}

/// @return pattern character in standard style

inline char Sequence::getPatternCharacter() const
{
	return getPadding() ? '#' : '@';
}

/// @return a string pattern using standard style

inline std::string Sequence::getStandardPattern() const
{
	return getPrefix() + std::string( getPadding() ? getPadding() : 1, getPatternCharacter() ) + getSuffix();
}

inline std::string Sequence::getAbsoluteStandardPattern() const
{
	return (getAbsoluteDirectory() / getStandardPattern() ).string();
}

/// @return a string pattern using C Style

inline std::string Sequence::getCStylePattern() const
{
	if( getPadding() )
		return getPrefix() + "%0" + boost::lexical_cast<std::string > ( getPadding() ) + "d" + getSuffix();
	else
		return getPrefix() + "%d" + getSuffix();
}

inline std::string Sequence::getAbsoluteCStylePattern() const
{
	return (getDirectory() / getCStylePattern() ).string();
}

inline std::pair<Time, Time> Sequence::getRange() const
{
	return std::pair<Time, Time > ( getFirstTime(), getLastTime() );
}

inline std::size_t Sequence::getStep() const
{
	return _step;
}

inline Time Sequence::getFirstTime() const
{
	return _firstTime;
}

inline Time Sequence::getLastTime() const
{
	return _lastTime;
}

inline std::size_t Sequence::getDuration() const
{
	return getLastTime() - getFirstTime() + 1;
}

inline Time Sequence::getNbFiles() const
{
	return _nbFiles;
}

inline std::size_t Sequence::getPadding() const
{
	return _padding;
}

inline bool Sequence::isStrictPadding() const
{
	return _strictPadding;
}

inline bool Sequence::hasMissingFile() const
{
	return getNbMissingFiles() != 0;
}

inline std::size_t Sequence::getNbMissingFiles() const
{
	if( !getStep() )
		return 0;
	return (( ( getLastTime() - getFirstTime() ) / getStep() ) + 1 ) -getNbFiles();
}

/// @brief filename without frame number

inline std::string Sequence::getIdentification() const
{
	return _prefix + _suffix;
}

inline std::string Sequence::getPrefix() const
{
	return _prefix;
}

inline std::string Sequence::getSuffix() const
{
	return _suffix;
}

