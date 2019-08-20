#!/bin/bash

cd /opt/sequenceparser
cmake . -DCMAKE_INSTALL_PREFIX=/opt/sequenceparser/install -DBOOST_ROOT=/usr/local/include/boost/
make
make install
