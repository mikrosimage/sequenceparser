#include "FrameRange.hpp"

#include <algorithm>
#include <sstream>


namespace sequenceParser {

std::string FrameRange::string() const
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const FrameRange& range)
{
	os << range.first;
	if( range.first == range.last )
		return os;
	os << ":" << range.last;

	if( range.step != 1 )
		os << "x" << range.step;

	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<FrameRange>& frameRanges)
{
	// to ensure that "ranges" size is > 0 as we use unsigned
	if( frameRanges.empty() )
		return os;

	std::vector<FrameRange>::const_iterator it = frameRanges.begin();
	for( std::size_t i = 0; i < frameRanges.size() - 1; ++i, ++it )
	{
		os << *it << ",";
	}
	os << *it;
	return os;
}

std::ostream& operator<<(std::ostream& os, const FrameRangesView& frameRanges)
{
	os << frameRanges.getFrameRanges();
	return os;
}

std::vector<FrameRange> extractFrameRanges( const std::vector<Time>& times )
{
	std::vector<FrameRange> res;
	if( times.empty() )
		return res;
	if( times.size() == 1 )
	{
		res.push_back(FrameRange(times.front()));
		return res;
	}
	res.push_back( FrameRange(times[0], times[1], std::max(Time(1), times[1]-times[0])) );
	if( times.size() == 2 )
	{
		return res;
	}

	std::vector<Time>::const_iterator itPrev = times.begin();
	std::vector<Time>::const_iterator itEnd = times.end();
	std::vector<Time>::const_iterator it = itPrev;
	++it;

	itPrev = it;
	++it;

	for( ; it != itEnd; itPrev = it, ++it )
	{
		Time newStep = *it - *itPrev;
		FrameRange& prevRange = res.back();
		if( prevRange.step == newStep )
		{
			// same step as previous range, so update it.
			prevRange.last = *it;
		}
		else if( prevRange.getNbFrames() == 1 )
		{
			// the previous range only contains one frame (without step)
			// so update it with the new step
			prevRange.last = *it;
			prevRange.step = newStep;

			// The range has only 2 frames.
		}
		else if( prevRange.getNbFrames() == 2 )
		{
			// The previous range has only 2 frames, so it's not really a range...
			// So steal the frame of the previous range.
			FrameRange newFrameRange(prevRange.last, *it, newStep);

			// Previous range is a still frame
			prevRange.last = prevRange.first;
			prevRange.step = 1;

			res.push_back( newFrameRange );
		}
		else
		{
			// The previous range is complete.
			// Create a new one.
			res.push_back( FrameRange(*it, *it) );
		}
	}

	FrameRange& lastRange = res.back();
	if( lastRange.getNbFrames() == 2 )
	{
		// If the last range has only 2 frames, so it's not really a range...
		// Split in 2 ranges of 1 frame.
		FrameRange newFrameRange(lastRange.last, lastRange.last, 1);
		lastRange.last = lastRange.first;
		lastRange.step = 1;

		res.push_back( newFrameRange );
	}
	return res;
}

std::size_t FrameRangesView::size() const
{
	std::size_t s = 0;
	BOOST_FOREACH( const FrameRange& frameRange, _data )
	{
		s += frameRange.getNbFrames();
	}
	return s;
}

std::string FrameRangesView::string() const
{
	std::ostringstream ss;
	ss << *this;
	return ss.str();
}

std::size_t FrameRangesSubView::size() const
{
	std::size_t s = 0;
	BOOST_FOREACH( const FrameRange& frameRange, _data )
	{
		// TODO: is > _firstTime and < _lastTime
		s += frameRange.getNbFrames();
	}
	return s;
}

FrameRangesSubView::EFrameStatus FrameRangesSubView::findGreaterOrEqualFrameRange( std::vector<FrameRange>::const_iterator& outIt, const Time time ) const
{
	outIt = _data.begin();
	std::vector<FrameRange>::const_iterator itEnd = _data.end();
	if( outIt == itEnd )
		return eFrameStatusNoFrameRange;
	if( time < outIt->first )
		return eFrameStatusBeforeAll;

	for(; outIt != itEnd; ++outIt)
	{
		if( time < outIt->first )
			return eFrameStatusBetweenRange;
		if( time >= outIt->first && time <= outIt->last )
			return eFrameStatusInRange;
	}
	return eFrameStatusAfterAll;
}

FrameRangesSubView::const_iterator FrameRangesSubView::begin() const
{
	std::vector<FrameRange>::const_iterator it;
	EFrameStatus frameStatus = findGreaterOrEqualFrameRange( it, _firstTime );

	switch( frameStatus )
	{
		case eFrameStatusInRange:
		{
			Time index = std::ceil((_firstTime - it->first) / (double)(it->step));
			return const_iterator(it, index);
		}
		case eFrameStatusBetweenRange:
		case eFrameStatusBeforeAll:
		case eFrameStatusAfterAll:
			return const_iterator(it, 0);
		case eFrameStatusNoFrameRange:
			return const_iterator(_data.begin(), 0);
	}
}

FrameRangesSubView::const_iterator FrameRangesSubView::end() const
{
	std::vector<FrameRange>::const_iterator it;
	EFrameStatus frameStatus = findGreaterOrEqualFrameRange( it, _lastTime );

	switch( frameStatus )
	{
		case eFrameStatusInRange:
		{
			Time index = std::ceil((_lastTime - it->first) / (double)(it->step));
			return ++const_iterator(it, index);
		}
		case eFrameStatusBetweenRange:
		case eFrameStatusBeforeAll:
		case eFrameStatusAfterAll:
			return const_iterator(it, 0);
		case eFrameStatusNoFrameRange:
			return const_iterator(_data.begin(), 0);
	}
}

}
