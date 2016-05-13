# SequenceParser

SequenceParser library is a library to detect sequences, files and folders.

[![Build Status](https://travis-ci.org/mikrosimage/sequenceparser.svg?branch=master)](https://travis-ci.org/mikrosimage/sequenceparser)
[![Coverage Status](https://coveralls.io/repos/github/mikrosimage/sequenceparser/badge.svg?branch=develop)](https://coveralls.io/github/mikrosimage/sequenceparser?branch=develop)  


#### Basics
* C++ library
* Java and Python bindings generated with SWIG
* multiplateform (Linux, MAC, Windows)
* the main idea is to handle ambiguous cases of sequence detection with options when browse disk

#### Licence
See [**COPYING.md**](COPYING.md)

#### Compilation
See [**INSTALL.md**](INSTALL.md)

#### External Libraries
* Boost
version 1.48.0 or upper
components regex, system, filesystem, locale
* Swig ( for Python/Java Binding )
1.3.36 or upper
* Python
version 2.6 or upper

#### Tests
* pyTest using nosetests

#### Definitions
* Sequence
it's many files with a common pattern of numbers
* File
it's all file without numbers in the filname and sequences with only one file
* Folders
Just folders
