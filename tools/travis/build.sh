#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Create directory of build
mkdir ${SEQUENCEPARSER_BUILD}
cd ${SEQUENCEPARSER_BUILD}

# Build sequenceParser
cmake --version
cmake .. -DCMAKE_INSTALL_PREFIX=${SEQUENCEPARSER_INSTALL} -DSEQUENCEPARSER_PYTHON_VERSION=${PYTHON_VERSION}
make -k
make install
cd ..
