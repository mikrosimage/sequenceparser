
inline std::string Sequence::getFirstFilename() const
{
	return getFilenameAt( getFirstTime() );
}

inline std::string Sequence::getLastFilename() const
{
	return getFilenameAt( getLastTime() );
}

inline char Sequence::getPatternCharacter() const
{
	return getFixedPadding() ? '#' : '@';
}

inline std::string Sequence::getFilenameWithStandardPattern() const
{
	return getPrefix() + std::string( getFixedPadding() ? getFixedPadding() : 1, getPatternCharacter() ) + getSuffix();
}

inline std::string Sequence::getFilenameWithPrintfPattern() const
{
	std::string paddingStr = "%";
	if( getFixedPadding() )
		paddingStr += "0" + boost::lexical_cast<std::string>( getFixedPadding() );
	paddingStr += "d";
	return getPrefix() + paddingStr + getSuffix();
}

inline std::pair<Time, Time> Sequence::getGlobalRange() const
{
	return std::pair<Time, Time > ( getFirstTime(), getLastTime() );
}

inline Time Sequence::getFirstTime() const
{
	return _ranges.front().first;
}

inline Time Sequence::getLastTime() const
{
	return _ranges.back().last;
}

inline std::size_t Sequence::getDuration() const
{
	return getLastTime() - getFirstTime() + 1;
}

inline std::size_t Sequence::getFixedPadding() const
{
	return _fixedPadding;
}

inline std::size_t Sequence::getMaxPadding() const
{
	return _maxPadding;
}

inline bool Sequence::hasMissingFile() const
{
	return _ranges.size() != 1 || _ranges.front().step != 1;
}

inline std::size_t Sequence::getNbMissingFiles() const
{
	return (( getLastTime() - getFirstTime() ) + 1 ) - getNbFiles();
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

