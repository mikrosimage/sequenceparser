#ifndef _SEQUENCE_PARSER_FRAMERANGE_HPP_
#define _SEQUENCE_PARSER_FRAMERANGE_HPP_

#include "commonDefinitions.hpp"

#include <boost/foreach.hpp>
#include <iostream>


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
	Time at(Time index) const
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


class FrameRangesView
{
public:
	FrameRangesView( const std::vector<FrameRange>& data )
	: _data(data)
	{}

#ifndef SWIG
	class const_iterator
	{
	public:
		typedef const_iterator self_type;
		typedef Time value_type;
		typedef Time reference;
		typedef Time* pointer;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef int difference_type;
		
		const_iterator(std::vector<FrameRange>::const_iterator rangeIterator, std::ssize_t index)
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
				return _rangeIterator->at(_rangeIterator->getNbFrames() + _index);
			
			return _rangeIterator->at(_index);
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
		
	private:
		std::vector<FrameRange>::const_iterator _rangeIterator;
		std::ssize_t _index;
	};

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


}

#endif
