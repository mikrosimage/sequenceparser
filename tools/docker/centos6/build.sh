#!/bin/bash

cd /opt/sequenceparser
cmake . -DCMAKE_INSTALL_PREFIX=/opt/sequenceparser/centos6 -DBOOST_ROOT=/usr/include/boost/
make
make install
