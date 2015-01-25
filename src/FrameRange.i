%include "common.i"

%{
#include "FrameRange.hpp"
%}

%include "FrameRange.hpp"

%extend sequenceParser::FrameRange
{
    %pythoncode
	{
        def __str__(self):
            return self.string()
    }
}

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

