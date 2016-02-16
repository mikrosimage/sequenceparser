#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# capture coverage info
lcov --capture --directory ${SEQUENCEPARSER_BUILD} --output-file coverage.info

# filter out system and test code
lcov --remove coverage.info '/usr/*' '*/*PYTHON_wrap.*' --output-file coverage.info

# debug before upload
lcov --list coverage.info

# uploads to coveralls
coveralls-lcov --repo-token ugBQ3ApyvqOygKt6AL6LemfMkOyTpbwpg coverage.info
