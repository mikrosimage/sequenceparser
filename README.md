# SequenceParser

SequenceParser library is a C++ library to detect sequences, files and folders.

#### Licence
See [**COPYING.md**](COPYING.md)COPYING files

#### Compilation
See [**INSTALL.md**](INSTALL.md)INSTALL file

#### Continuous Integration
###### Travis
[![Build Status](https://travis-ci.org/mikrosimage/sequenceparser.svg?branch=master)](https://travis-ci.org/mikrosimage/sequenceparser)

#### External Libraries
###### Boost
version 1.48.0 or upper
components regex, system, filesystem, locale
###### Swig ( for Python/Java Binding )
1.3.36 or upper
###### Python
version 2.6 or upper

#### Tests
pyTest using nosetests
c++ test using Boost Unit Tests (outdated and not compiled)

#### Definitions
###### Sequence
it's many files with a common pattern of numbers
###### File
it's all file without numbers in the filname and sequences with only one file
###### Folders
Just folders
