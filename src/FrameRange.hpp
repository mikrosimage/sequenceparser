#ifndef _SEQUENCE_PARSER_FRAMERANGE_HPP_
#define _SEQUENCE_PARSER_FRAMERANGE_HPP_

#include "commonDefinitions.hpp"

#include <boost/foreach.hpp>

#include <vector>
#include <iostream>
#include <cmath>


namespace sequenceParser {

class FrameRange
{
public:
	FrameRange(Time first)
	: first(first)
	, last(first)
	, step(1)
	{}
	FrameRange(Time first, Time last, Time step = 1)
	: first(first)
	, last(last)
	, step(step)
	{}
	Time atIndex(Time index) const
	{
		return first + index * step;
	}
	Time getNbFrames() const
	{
		if( step == 0 )
			return 1;
		return ((last - first) / step) + 1;
	}
	
	bool operator==( const FrameRange& other ) const
	{
		return (first == other.first) &&
		       (last == other.last) &&
		       (step == other.step);
	}
	std::ostream& getCout( std::ostream& os ) const;

	Time first, last, step;
};

inline std::ostream& operator<<(std::ostream& os, const FrameRange& range)
{
	return range.getCout(os);
}

class FrameRangesConstIterator
{
public:
	typedef FrameRangesConstIterator self_type;
	typedef Time value_type;
	typedef Time reference;
	typedef Time* pointer;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef int difference_type;

	FrameRangesConstIterator(std::vector<FrameRange>::const_iterator rangeIterator, std::ssize_t index)
		: _rangeIterator(rangeIterator)
		, _index(index)
	{}
	self_type operator++()
	{
		if( _index < _rangeIterator->getNbFrames() - 1 )
		{
			++_index;
		}
		else
		{
			_index = 0;
			++_rangeIterator;
		}
		return *this;
	}
	self_type operator++(int junk)
	{
		self_type i = *this;
		++(*this);
		return i;
	}
	self_type operator--()
	{
		if( _index > 0 )
		{
			--_index;
		}
		else if( _index == 0 )
		{
			--_rangeIterator;
			_index = -1;
		}
		else if( std::abs(_index) < _rangeIterator->getNbFrames() )
		{
			--_index;
		}
		else
		{
			--_rangeIterator;
			_index = -1;
		}
		return *this;
	}
	self_type operator--(int junk)
	{
		self_type i = *this;
		--(*this);
		return i;
	}
	value_type operator*()
	{
		if( _index < 0 )
			return _rangeIterator->atIndex(_rangeIterator->getNbFrames() + _index);

		return _rangeIterator->atIndex(_index);
	}
	std::ssize_t getRealIndex() const 
	{
		if( _index < 0 )
			return _rangeIterator->getNbFrames() + _index;
		return _index;
	}
	bool operator==(const self_type& other) const
	{
		return _rangeIterator == other._rangeIterator && (_index == other._index || getRealIndex() == other.getRealIndex());
	}
	bool operator!=(const self_type& other) const
	{
		return ! operator==(other);
	}

public:
	std::vector<FrameRange>::const_iterator _rangeIterator;
	std::ssize_t _index;
};

class FrameRangesView
{
public:
	typedef FrameRangesConstIterator const_iterator;
	
	FrameRangesView( const std::vector<FrameRange>& data )
	: _data(data)
	{}

#ifndef SWIG
	
	std::size_t size() const
	{
		std::size_t s = 0;
		BOOST_FOREACH( const FrameRange& frameRange, _data )
		{
			s += frameRange.getNbFrames();
		}
		return s;
	}

	const_iterator begin() const
	{
		return const_iterator(_data.begin(), 0);
	}

	const_iterator end() const
	{
		return const_iterator(_data.end(), 0);
	}
#endif

private:
	const std::vector<FrameRange>& _data;
};

class FrameRangesSubView
{
public:
	typedef FrameRangesConstIterator const_iterator;
	
	FrameRangesSubView( const std::vector<FrameRange>& data, const Time firstTime, const Time lastTime )
	: _data(data)
	, _firstTime(firstTime)
	, _lastTime(lastTime)
	{}

#ifndef SWIG
	std::size_t size() const
	{
		std::size_t s = 0;
		BOOST_FOREACH( const FrameRange& frameRange, _data )
		{
			s += frameRange.getNbFrames();
		}
		return s;
	}
	
	enum EFrameStatus {
		eFrameStatusInRange,
		eFrameStatusBetweenRange,
		eFrameStatusBeforeAll,
		eFrameStatusAfterAll,
		eFrameStatusNoFrameRange
	};
	
	EFrameStatus findGreaterOrEqualFrameRange( std::vector<FrameRange>::const_iterator& outIt, const Time time ) const
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
	
	const_iterator begin() const
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
	
	const_iterator end() const
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
#endif

private:
	const std::vector<FrameRange>& _data;
	Time _firstTime;
	Time _lastTime;
};


}

#endif
