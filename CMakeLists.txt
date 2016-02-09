cmake_minimum_required(VERSION 2.8) 

project(sequenceParser)

# Get SequenceParser versions
# SEQUENCEPARSER_VERSION_MAJOR
# SEQUENCEPARSER_VERSION_MINOR
# SEQUENCEPARSER_VERSION_MICRO
# SEQUENCEPARSER_VERSION
file(STRINGS "src/sequenceParser/common.hpp" _avtranscoder_VERSION_HPP_CONTENTS REGEX "#define SEQUENCEPARSER_VERSION_")
foreach(v MAJOR MINOR MICRO)
  if("${_avtranscoder_VERSION_HPP_CONTENTS}" MATCHES "#define SEQUENCEPARSER_VERSION_${v} ([0-9]+)")
    set(SEQUENCEPARSER_VERSION_${v} "${CMAKE_MATCH_1}")
  else()
    set(SEQUENCEPARSER_RETRIEVE_VERSION_FAILED 1)
  endif()
endforeach()
unset(_avtranscoder_VERSION_HPP_CONTENTS)

set(SEQUENCEPARSER_VERSION "${SEQUENCEPARSER_VERSION_MAJOR}.${SEQUENCEPARSER_VERSION_MINOR}.${SEQUENCEPARSER_VERSION_MICRO}")

if(SEQUENCEPARSER_RETRIEVE_VERSION_FAILED)
  message(SEND_ERROR "Failed to retrieve SequenceParser version: ${SEQUENCEPARSER_VERSION}")
else()
  message(STATUS "SequenceParser version is ${SEQUENCEPARSER_VERSION}")
endif()

# Diplay commands being ran by CMake
set(CMAKE_VERBOSE_MAKEFILE OFF)

# The install rule does not depend on all, i.e. everything will not be built before installing
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# CPP flags on debug / release mode
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")
endif()

set(SEQUENCEPARSER_SRC_PATH "${PROJECT_SOURCE_DIR}/src")

# Build library
add_subdirectory(src)
# Build boost tests
#add_subdirectory(test)
