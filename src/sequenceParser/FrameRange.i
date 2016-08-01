%include "common.i"

%{
#include "sequenceParser/FrameRange.hpp"
%}

#ifdef SWIGJAVA

%rename(toString) sequenceParser::FrameRange::string;
%rename(equals) sequenceParser::FrameRange::operator==;

%ignore sequenceParser::FrameRangesConstIterator::operator++;
%ignore sequenceParser::FrameRangesConstIterator::operator--;
%rename(equals) sequenceParser::FrameRangesConstIterator::operator==;
%ignore sequenceParser::FrameRangesConstIterator::operator!=;

#elif SWIGPYTHON

%rename(__str__) sequenceParser::FrameRange::string;
%rename(__eq__) sequenceParser::FrameRange::operator==;

%rename(__next__) sequenceParser::FrameRangesConstIterator::operator++;
%rename(__previous__) sequenceParser::FrameRangesConstIterator::operator--;
%rename(__eq__) sequenceParser::FrameRangesConstIterator::operator==;
%rename(__ne__) sequenceParser::FrameRangesConstIterator::operator!=;

#endif

%include "FrameRange.hpp"

#ifdef SWIGPYTHON

%pythoncode
{
    class PyFrameRangesConstIterator:
        def __init__(self, it, itEnd):
            self.it = it
            self.itEnd = itEnd
        def __iter__(self):
            return self
        def __next__(self):
            '''Python3 iterator'''
            self.it = self.it.next()
            if self.it == self.itEnd:
                raise StopIteration
            return self.it.value()
        def next(self):
            '''Python2 iterator'''
            return self.__next__()
}

%extend sequenceParser::FrameRangesView
{
    %pythoncode
	{
        def __iter__(self):
            return PyFrameRangesConstIterator(self.begin().previous(), self.end())
        def __str__(self):
            return self.string()
    }
}

%extend sequenceParser::FrameRangesSubView
{
    %pythoncode
	{
        def __iter__(self):
            return PyFrameRangesConstIterator(self.begin().previous(), self.end())
        def __str__(self):
            return self.string()
    }
}

#endif
