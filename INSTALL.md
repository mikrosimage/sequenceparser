# How to install sequenceParser

SequenceParser uses CMake build system.

#### To build
```
cmake .
make
make install
```

#### External Libraries
* Boost
version 1.48.0 or upper
components regex, system, filesystem, locale
* Swig ( for Python/Java Binding )
1.3.36 or upper
* Python
version 2.6 or upper

#### To use specific library
###### Boost
Add ```-DBOOST_ROOT=/path/to/your/boost/install``` to the cmake command.

#### Tested compilers
###### Linux
GCC 4.4.1 on OpenSUSE 11.2 64bits
###### MacOS
###### Windows
