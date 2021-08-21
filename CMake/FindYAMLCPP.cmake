# Locate the yaml_cpp library
#
# This module defines the following variables:
#
# YAMLCPP_RELEASE the name of the Release library;
# YAMLCPP_DEBUG the name of the Debug library;
# YAMLCPP_INCLUDE_DIR where to find YAMLCpp include files.
# YAMLCPP_FOUND true if both the YAMLCPP_LIBRARY and YAMLCPP_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called YAMLCPP_ROOT which points to the root of the YAMLCpp library
# installation.

set( _YAMLCPP_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/include")
set( _YAMLCPP_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib" )

# Check environment for root search directory
set( _YAMLCPP_ENV_ROOT $ENV{YAMLCPP_ROOT} )
if( NOT YAMLCPP_ROOT AND _YAMLCPP_ENV_ROOT )
	set(YAMLCPP_ROOT ${_YAMLCPP_ENV_ROOT})
endif()

# Put user specified location at beginning of search
if( YAMLCPP_ROOT )
	list( INSERT _YAMLCPP_HEADER_SEARCH_DIRS 0 "${YAMLCPP_ROOT}/include" )
	list( INSERT _YAMLCPP_LIB_SEARCH_DIRS 0 "${YAMLCPP_ROOT}/lib/Debug" "${YAMLCPP_ROOT}/lib/Release" )
endif()

# Search for the header
FIND_PATH(YAMLCPP_INCLUDE_DIR "yaml-cpp/yaml.h"
PATHS ${_YAMLCPP_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(YAMLCPP_DEBUG NAMES yaml-cppd
PATHS ${_YAMLCPP_LIB_SEARCH_DIRS} )

FIND_LIBRARY(YAMLCPP_RELEASE NAMES yaml-cpp
PATHS ${_YAMLCPP_LIB_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YAMLCPP DEFAULT_MSG
YAMLCPP_DEBUG YAMLCPP_RELEASE YAMLCPP_INCLUDE_DIR)
