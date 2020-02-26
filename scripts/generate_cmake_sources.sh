#!/bin/sh

WORKING_DIR=${1:-..}
TARGET_NAME=${2:-Monocle}
SOURCES_VAR=${TARGET_NAME}_SOURCES

WINDOWS_ONLY_SUBPATH='Windows/'
LINUX_ONLY_SUBPATH='Linux/'
MACOS_ONLY_SUBPATH='MacOS/'

cd "$WORKING_DIR"

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find source/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" | sed 's/ /\\ /g')
CROSSPLATFORM_SOURCES=$( echo "$BASE_SOURCES" | grep -Fv -e "$WINDOWS_ONLY_SUBPATH" -e "$LINUX_ONLY_SUBPATH" -e "$MACOS_ONLY_SUBPATH" )

WIN32_SOURCES=$( echo "$BASE_SOURCES" | grep "$WINDOWS_ONLY_SUBPATH")
LINUX_SOURCES=$( echo "$BASE_SOURCES" | grep "$LINUX_ONLY_SUBPATH" )
MACOS_SOURCES=$( echo "$BASE_SOURCES" | grep "$MACOS_ONLY_SUBPATH")


echo "

set($SOURCES_VAR
	$CROSSPLATFORM_SOURCES
	)
	
if(WIN32)
	set($SOURCES_VAR
	
	\${$SOURCES_VAR} 
	
	$WIN32_SOURCES
	)
	
elseif(APPLE)
	set($SOURCES_VAR
	
	\${$SOURCES_VAR} 
	
	$MACOS_SOURCES
	)
	
elseif(UNIX)
	set($SOURCES_VAR
	
	\${$SOURCES_VAR} 
	
	$LINUX_SOURCES
	)
	
	
endif()" > CMakeLists_sources.cmake