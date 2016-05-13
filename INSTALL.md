# How to install sequenceParser

SequenceParser uses CMake build system.

#### To build
```
cmake .
make
make install
```

#### External Libraries
###### Boost
Version 1.48.0 or upper  
Components regex, system, filesystem, locale

######  Swig
Version 1.3.36 or upper  
(for Python/Java Binding)

#### To use specific library
###### Boost
```
cmake . -DBOOST_ROOT=/path/to/your/boost/install
```

#### Tested compilers
###### Linux
* GCC 4.4.1 on OpenSUSE 11.2 64bits
* GCC 4.6.2 on Fedora 20 64bits

###### MacOS

###### Windows


#### Launch tests
###### Python tests
Using nosetests:
```
nosetests test/pyTest
```
