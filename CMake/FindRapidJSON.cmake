# Locate the yaml_cpp library
#
# This module defines the following variables:
#
# RAPIDJSON_INCLUDE_DIR where to find RAPIDJSON include files.
# RAPIDJSON_FOUND true if RAPIDJSON_INCLUDE_DIR has been found.
#
# To help locate the library and include file, you can define a
# variable called RAPIDJSON_ROOT which points to the root of the RAPIDJSON library
# installation.

set( _RAPIDJSON_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/include")
set( _RAPIDJSON_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib" )

# Check environment for root search directory
set( _RAPIDJSON_ENV_ROOT $ENV{RAPIDJSON_ROOT} )
if( NOT RAPIDJSON_ROOT AND _RAPIDJSON_ENV_ROOT )
	set(RAPIDJSON_ROOT ${_RAPIDJSON_ENV_ROOT})
endif()

# Put user specified location at beginning of search
if( RAPIDJSON_ROOT )
	list( INSERT _RAPIDJSON_HEADER_SEARCH_DIRS 0 "${RAPIDJSON_ROOT}" )
endif()

# Search for the header
FIND_PATH(RapidJSON_INCLUDE_DIR "rapidjson/rapidjson.h"
PATHS ${_RAPIDJSON_HEADER_SEARCH_DIRS} )


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RapidJSON DEFAULT_MSG RapidJSON_INCLUDE_DIR)
