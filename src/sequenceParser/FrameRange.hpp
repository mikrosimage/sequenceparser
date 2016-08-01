#ifndef _SEQUENCE_PARSER_FRAMERANGE_HPP_
#define _SEQUENCE_PARSER_FRAMERANGE_HPP_

#include "common.hpp"

#include <boost/foreach.hpp>
#include <boost/assert.hpp>

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
	{
		BOOST_ASSERT( step >= 1 );
	}
	inline Time atIndex(Time index) const
	{
		return first + index * step;
	}
	inline Time getNbFrames() const
	{
		BOOST_ASSERT( step >= 1 );
		return ((last - first) / step) + 1;
	}
	inline bool operator==( const FrameRange& other ) const
	{
		return (first == other.first) &&
		       (last == other.last) &&
		       (step == other.step);
	}
	std::string string() const;

public:
	Time first;
	Time last;
	Time step;  // 1 >= step > N
};

#ifndef SWIG
/**
 * @brief Convert FrameRange into a string.
 * FrameRange(1, 10, 2) => "1-10x2".
 * FrameRange(1, 10, 1) => "1-10".
 * FrameRange(1,  1, 1) => "1".
 */
std::ostream& operator<<(std::ostream& os, const FrameRange& range);


/// @brief Convert list of FrameRange into a string, like "1-10x2, 20-35, 55, 57".
std::ostream& operator<<(std::ostream& os, const std::vector<FrameRange>& range);
#endif

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
	inline self_type operator++()
	{
		if( _index >= 0 && _index < _rangeIterator->getNbFrames() - 1 )
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
	inline self_type next()
	{
		return this->operator++();
	}
	inline self_type operator++(int junk)
	{
		self_type i = *this;
		++(*this);
		return i;
	}
	inline self_type operator--()
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
	inline self_type operator--(int junk)
	{
		self_type i = *this;
		--(*this);
		return i;
	}
	inline self_type previous()
	{
		return this->operator--();
	}
	inline value_type operator*()
	{
		if( _index < 0 )
			return _rangeIterator->atIndex(_rangeIterator->getNbFrames() + _index);

		return _rangeIterator->atIndex(_index);
	}
	inline value_type value()
	{
		return this->operator*();
	}
	inline std::ssize_t getRealIndex() const 
	{
		if( _index < 0 )
			return _rangeIterator->getNbFrames() + _index;
		return _index;
	}
	inline bool operator==(const self_type& other) const
	{
		return _rangeIterator == other._rangeIterator && (_index == other._index || getRealIndex() == other.getRealIndex());
	}
	inline bool operator!=(const self_type& other) const
	{
		return ! operator==(other);
	}

public:
	std::vector<FrameRange>::const_iterator _rangeIterator;
	std::ssize_t _index;
};

#ifndef SWIG
std::vector<FrameRange> extractFrameRanges( const std::vector<Time>& times );
#endif

class FrameRangesView
{
public:
	typedef FrameRangesConstIterator const_iterator;

	FrameRangesView( const std::vector<FrameRange>& data )
	: _data(data)
	{}

	std::size_t size() const;

	inline const_iterator begin() const
	{
		return const_iterator(_data.begin(), 0);
	}
	inline const_iterator end() const
	{
		return const_iterator(_data.end(), 0);
	}
	inline const std::vector<FrameRange>& getFrameRanges() const
	{
		return _data;
	}
	std::string string() const;

private:
	const std::vector<FrameRange>& _data;
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const FrameRangesView& frameRanges);
#endif

class FrameRangesSubView
{
public:
	typedef FrameRangesConstIterator const_iterator;

	FrameRangesSubView( const std::vector<FrameRange>& data, const Time firstTime, const Time lastTime )
	: _data(data)
	, _firstTime(firstTime)
	, _lastTime(lastTime)
	{}

	std::size_t size() const;

	enum EFrameStatus {
		eFrameStatusInRange,
		eFrameStatusBetweenRange,
		eFrameStatusBeforeAll,
		eFrameStatusAfterAll,
		eFrameStatusNoFrameRange
	};

#ifndef SWIG
	EFrameStatus findGreaterOrEqualFrameRange( std::vector<FrameRange>::const_iterator& outIt, const Time time ) const;
#endif

	const_iterator begin() const;

	const_iterator end() const;

private:
	const std::vector<FrameRange>& _data;
	Time _firstTime;
	Time _lastTime;
};


}

#endif
