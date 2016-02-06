#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# set env
export PYTHONPATH=$PYTHONPATH:${SEQUENCEPARSER_INSTALL}/lib/python${PYTHON_VERSION}/site-packages

# nosetests
if [[ ${PYTHON_VERSION} == "2.7" ]]; then
    nosetests ${TRAVIS_BUILD_DIR}/pyTest -v
elif [[ ${PYTHON_VERSION} == "3.4" ]]; then
    nosetests3 ${TRAVIS_BUILD_DIR}/pyTest -v
fi
