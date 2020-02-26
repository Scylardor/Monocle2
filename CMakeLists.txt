cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(PROJECT_NAME Monocle)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR} source)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR} vendor) # The directory of all third party libraries embedded into the project

project(${PROJECT_NAME} LANGUAGES CXX)

option(${PROJECT_NAME}_BUILD_SHARED_LIB "If ON, Monocle will be built as a shared library." OFF)

option(${PROJECT_NAME}_USE_ASSERTS "If ON, Monocle asserts will be defined. Otherwise, they will become no-ops." ON)
option(${PROJECT_NAME}_USE_STL "If ON, Monocle containers will use STL containers under the hood." ON)
option(${PROJECT_NAME}_USE_GLFW "If ON, Monocle will compile with the GLFW library." ON)
option(${PROJECT_NAME}_BUILD_UNIT_TESTS "If ON, Monocle will build the Unit Tests project." OFF)
option(${PROJECT_NAME}_BUILD_SANDBOX "If ON, Monocle will build a test project." ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This .cmake includes and defines the ${PROJECT_NAME}_SOURCES variable !
# It is generated by the generate_cmake_sources script.
include(CMakeLists_sources.cmake)

if(${PROJECT_NAME}_BUILD_SHARED_LIB)
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES} )
else()
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} )
endif()

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR
# to recreate the folder tree with filters within Visual Studio for example.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${PROJECT_NAME}_SOURCES})

# Definitions tweaking
if(${PROJECT_NAME}_USE_STL)
	add_definitions(-DMOE_STD_SUPPORT)
endif()
if (WIN32)
	add_definitions(-DMOE_WINDOWS)
elseif(APPLE)
	add_definitions(-DMOE_MACOS)
elseif(UNIX)
	add_definitions(-DMOE_LINUX)
endif()
if(${PROJECT_NAME}_USE_ASSERTS)
	add_definitions(-DMOE_USE_ASSERTS)
endif()
# Definitions tweaking End

# Add here all the headers that should be in precompiled header file.
if(${PROJECT_NAME}_USE_STL)
target_precompile_headers(${PROJECT_NAME}
  PUBLIC
    "${SOURCE_DIR}/pch.h"
  PRIVATE
    <unordered_map>
	<string>
	<cstddef>
	<functional>
	<utility>
	<algorithm>
	<cstdio>
	<iostream>
	<memory>
)
else()
target_precompile_headers(${PROJECT_NAME}
  PUBLIC
    "${SOURCE_DIR}/pch.h"
  PRIVATE
)
endif()

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})
# Some libraries do not have an "include" directory, hence we need to set base vendor dir as an include dir.
target_include_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR})


if(${PROJECT_NAME}_USE_GLFW)
	set(GLFW_DIR ${VENDOR_DIR}/glfw)
	target_link_directories(${PROJECT_NAME} PUBLIC ${GLFW_DIR})
	target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)
	target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_DIR}/include")
	add_definitions(-DMOE_GLFW)
endif()

if(${PROJECT_NAME}_BUILD_UNIT_TESTS)
	add_subdirectory(UnitTests)
endif()

if(${PROJECT_NAME}_BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()