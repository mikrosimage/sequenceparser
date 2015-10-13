set(SEQUENCEPARSER_SRC_PATH "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE SEQUENCEPARSER_SRC_FILES "*.cpp" "*.hpp")

# Find boost
find_package(Boost 1.53.0
    COMPONENTS regex system filesystem locale REQUIRED)
if(NOT Boost_FOUND) 
    message(FATAL_ERROR "please set BOOST_ROOT environment variable to a proper boost install")
endif(NOT Boost_FOUND)

# Include sequenceParser and boost
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

# Create 'sequenceparser' static lib
add_library(sequenceparser-static STATIC ${SEQUENCEPARSER_SRC_FILES})
set_target_properties(sequenceparser-static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sequenceparser-static PROPERTIES OUTPUT_NAME sequenceParser)
target_link_libraries(sequenceparser-static ${Boost_LIBRARIES})

# Create 'sequenceparser' shared lib
add_library(sequenceparser-shared SHARED ${SEQUENCEPARSER_SRC_FILES})
set_target_properties(sequenceparser-shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sequenceparser-shared PROPERTIES OUTPUT_NAME sequenceParser)
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
install(TARGETS sequenceparser-static DESTINATION lib/ OPTIONAL)
install(TARGETS sequenceparser-shared DESTINATION lib/ OPTIONAL)

### SWIG
find_package(SWIG)
if(SWIG_FOUND)
	# Load Modules/UseSWIG.cmake
	include(${SWIG_USE_FILE})

	# Configure Swig
	set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/sequenceParser")
	set(SEQUENCEPARSER_BINDING_FILE "sequenceParser.i")
	set_source_files_properties(${SEQUENCEPARSER_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

	# Swig flags
	set(CMAKE_SWIG_FLAGS -c++ -fcompact -small -O -Wall)

	### PYTHON BINDING
	find_package(PythonLibs ${SEQUENCEPARSER_PYTHON_VERSION})
	if(PYTHONLIBS_FOUND)
		include_directories(${PYTHON_INCLUDE_PATH})

		# Swig flags
		if(${PYTHONLIBS_VERSION_STRING} VERSION_GREATER "3")
			set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -py3)
		endif()

		# Create '_sequenceparser' shared lib (python)
		swig_add_module(sequenceparser-py python ${SEQUENCEPARSER_BINDING_FILE})
		# For Python binding, need to compile the wrapper into a lib called "_<module_name>.so"
		set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES OUTPUT_NAME _sequenceParser)
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES SOVERSION ${SEQUENCEPARSER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES VERSION ${SEQUENCEPARSER_VERSION})
		endif()
		# Force the library to look in the project lib folder relatively
		set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
		if(APPLE)
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../../..")
		else()
			set_target_properties(${SWIG_MODULE_sequenceparser-py_REAL_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../../..:$ORIGIN")
		endif()
		swig_link_libraries(sequenceparser-py sequenceparser-shared ${PYTHON_LIBRARIES})

		# Get path to install python binding
		string(SUBSTRING ${PYTHONLIBS_VERSION_STRING} 0 3 PYTHON_VERSION)
		set(SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR "lib/python${PYTHON_VERSION}/site-packages/pySequenceParser/")

		# Install python interface
		set(SEQUENCEPARSER_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/sequenceParser.py")
		install(
			FILES ${SEQUENCEPARSER_PYTHON_BINDING_FILE} ${SEQUENCEPARSER_PYTHON_INIT_FILE}
			DESTINATION ${SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR}
			OPTIONAL
		)
		# Install python lib and __init__.py files
		install(TARGETS ${SWIG_MODULE_sequenceparser-py_REAL_NAME} DESTINATION "${SEQUENCEPARSER_PYTHON_MODULE_OUTPUT_DIR}" OPTIONAL)
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
		set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -package org.sequenceparser)

		# Create 'sequenceparser-java' shared lib
		swig_add_module(sequenceparser-java java ${SEQUENCEPARSER_BINDING_FILE})
		set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES OUTPUT_NAME sequenceParser-java)
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES SOVERSION ${SEQUENCEPARSER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES VERSION ${SEQUENCEPARSER_VERSION})
		endif()
		# Force the library to look in the project lib folder relatively
		set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
		if(APPLE)
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/..")
		else()
			set_target_properties(${SWIG_MODULE_sequenceparser-java_REAL_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/..:$ORIGIN")
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

		# Install java lib and jar files
		install(TARGETS ${SWIG_MODULE_sequenceparser-java_REAL_NAME} DESTINATION "lib/java" OPTIONAL)
		install(FILES ${SEQUENCEPARSER_JAR_PATH}/${SEQUENCEPARSER_JAR_NAME} DESTINATION "share/java/" OPTIONAL)
	else()
		message("JAVA not found, will not build java binding.")
	endif()

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)
