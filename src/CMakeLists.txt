set(SEQUENCEPARSER_SRC_PATH "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE SEQUENCEPARSER_SRC_FILES "*.cpp" "*.hpp")

# Find boost
find_package(Boost 1.55.0 
    COMPONENTS regex system filesystem locale REQUIRED)
if(NOT Boost_FOUND) 
    message(FATAL_ERROR "please set BOOST_ROOT environment variable to a proper boost install")
endif(NOT Boost_FOUND)

# Include sequenceParser and boost
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

# Create 'sequenceparser' static lib
add_library(sequenceparser-static STATIC ${SEQUENCEPARSER_SRC_FILES})
set_target_properties(sequenceparser-static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sequenceparser-static PROPERTIES OUTPUT_NAME sequenceparser)
target_link_libraries(sequenceparser-static ${Boost_LIBRARIES})

# Create 'sequenceparser' shared lib
add_library(sequenceparser-shared SHARED ${SEQUENCEPARSER_SRC_FILES})
set_target_properties(sequenceparser-shared PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
	set_target_properties(sequenceparser-shared PROPERTIES OUTPUT_NAME "sequenceparser-${SEQUENCEPARSER_VERSION}")
else()
	set_target_properties(sequenceparser-shared PROPERTIES OUTPUT_NAME sequenceparser)
endif()
set_target_properties(sequenceparser-shared PROPERTIES SOVERSION ${SEQUENCEPARSER_VERSION_MAJOR})
set_target_properties(sequenceparser-shared PROPERTIES VERSION ${SEQUENCEPARSER_VERSION})
target_link_libraries(sequenceparser-shared ${Boost_LIBRARIES})

### Install sequenceParser libs and include
install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.i"
)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
install(
	DIRECTORY "${LIBRARY_OUTPUT_PATH}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

### SWIG
find_package(SWIG)
if(SWIG_FOUND)
	# Load Modules/UseSWIG.cmake
	include(${SWIG_USE_FILE})

	# Configure Swig
	set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/sequenceParser")
	set(SEQUENCEPARSER_BINDING_FILE "sequenceParser.i")
	set_source_files_properties(${SEQUENCEPARSER_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

	### PYTHON BINDING
	find_package(PythonLibs)
	if(PYTHONLIBS_FOUND)
		include_directories(${PYTHON_INCLUDE_PATH})

		# Swig flags
		set(CMAKE_SWIG_FLAGS -c++ -fcompact)

		# Create '_sequenceparser' shared lib (python)
		swig_add_module(sequenceparser-py python ${SEQUENCEPARSER_BINDING_FILE})
		# For Python binding, need to compile the wrapper into a lib called "_<module_name>.so"
		set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES OUTPUT_NAME _sequenceparser)
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES SOVERSION ${SEQUENCEPARSER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES VERSION ${SEQUENCEPARSER_VERSION})
		endif()
		swig_link_libraries(sequenceparser-py sequenceparser-shared ${PYTHON_LIBRARIES})

		# Install python interface
		set(SEQUENCEPARSER_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/sequenceParser.py")
		set(SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/pySequenceParser/")
		install(
			FILES ${SEQUENCEPARSER_PYTHON_BINDING_FILE} ${SEQUENCEPARSER_PYTHON_INIT_FILE}
			DESTINATION ${SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR}
		)
		# Install __init__.py
		install(CODE "file(WRITE ${CMAKE_INSTALL_PREFIX}/${SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR}/__init__.py)")
	else()
		message("PYTHON not found, will not build python binding.")
	endif()

	### JAVA BINDING
	find_package(Java)
	find_package(JNI)
	if(JAVA_FOUND AND JNI_FOUND)
		include(UseJava)
		include_directories(${JAVA_INCLUDE_PATH})
		include_directories(${JNI_INCLUDE_DIRS})

		# Swig flags
		set(CMAKE_SWIG_FLAGS -c++ -fcompact -package org.sequenceparser)

		# Create 'sequenceparser-java' shared lib
		swig_add_module(sequenceparser-java java ${SEQUENCEPARSER_BINDING_FILE})
		if(WIN32)
			set_target_properties(sequenceparser-java PROPERTIES OUTPUT_NAME "sequenceparser-java-${SEQUENCEPARSER_VERSION}")
		endif()
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES SOVERSION ${SEQUENCEPARSER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES VERSION ${SEQUENCEPARSER_VERSION})
		endif()
		swig_link_libraries(sequenceparser-java sequenceparser-shared)

		# Post build of java binding : compile java files (generate class files) and create jar
		set(SEQUENCEPARSER_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/sequenceParser")
		set(SEQUENCEPARSER_JAVA_CLASS_OUTPUT_PATH "${SEQUENCEPARSER_JAVA_OUTPUT_PATH}/target")
		set(SEQUENCEPARSER_JAR_PATH "${PROJECT_BINARY_DIR}/src")
		set(SEQUENCEPARSER_JAR_NAME "sequenceparser-${SEQUENCEPARSER_VERSION}.jar")
		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${SEQUENCEPARSER_JAVA_CLASS_OUTPUT_PATH})
		
		ADD_CUSTOM_COMMAND(
			TARGET sequenceparser-java
			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} "-d" ${SEQUENCEPARSER_JAVA_CLASS_OUTPUT_PATH} "\"${SEQUENCEPARSER_JAVA_OUTPUT_PATH}\"/*.java"
			COMMAND ${Java_JAR_EXECUTABLE} cf ${SEQUENCEPARSER_JAR_NAME} "-C" ${SEQUENCEPARSER_JAVA_CLASS_OUTPUT_PATH} "."
		)

		# Install jar file
		install(FILES ${SEQUENCEPARSER_JAR_PATH}/${SEQUENCEPARSER_JAR_NAME} DESTINATION "share/java/")
	else()
		message("JAVA not found, will not build java binding.")
	endif()

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)
