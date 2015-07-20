# How to install sequenceParser

SequenceParser uses CMake build system.

#### To build
```
cmake .
make
make install
```

#### To use specific library
###### Boost
Add ```-DBOOST_ROOT=/path/to/your/boost/install``` to the cmake command.

#### Tested compilers
###### Linux
GCC 4.4.1 on OpenSUSE 11.2 64bits
###### MacOS
###### Windows
