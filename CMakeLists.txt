cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(PROJECT_NAME Monocle)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR} source)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR} vendor) # The directory of all third party libraries embedded into the project

project(${PROJECT_NAME} LANGUAGES CXX)

# Monocle internal features
option(${PROJECT_NAME}_USE_ASSERTS "If ON, Monocle asserts will be defined. Otherwise, they will become no-ops." ON)
option(${PROJECT_NAME}_USE_STL "If ON, Monocle containers will use STL containers under the hood." ON)
option(${PROJECT_NAME}_USE_GLM "If ON, Monocle will use GLM as underlying Math library." ON)
option(${PROJECT_NAME}_USE_STB_IMAGE_IMPORTER "If ON, Monocle will use STB as Image Importer." ON)
option(${PROJECT_NAME}_USE_ASSIMP_IMPORTER "If ON, Monocle will use Assimp as the 3D Object Importer." ON)


# Windowing APIs
option(${PROJECT_NAME}_USE_GLFW "If ON, Monocle will compile with the GLFW library." ON)
option(${PROJECT_NAME}_USE_WIN32 "If ON, Monocle will compile with the Win32 windowing and input library." OFF)

# Graphics APIs
option(${PROJECT_NAME}_USE_OPENGL "If ON, Monocle will compile OpenGL dependencies (RenderContext, applications, etc)." ON)
option(${PROJECT_NAME}_USE_VULKAN "If ON, Monocle will compile Vulkan dependencies (RenderContext, applications, etc)." OFF)
option(${PROJECT_NAME}_USE_DIRECTX "If ON, Monocle will compile Direct3D dependencies (RenderContext, applications , etc)." OFF)

# Monocle libraries to build
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "If ON, Monocle will build shared libraries instead of static libraries." OFF)

option(${PROJECT_NAME}_BUILD_CORE "If ON, Monocle will build the Core library." ON)
option(${PROJECT_NAME}_BUILD_INPUT "If ON, Monocle will build the Input library." ON)
option(${PROJECT_NAME}_BUILD_APPLICATION "If ON, Monocle will build the Application library." ON)
option(${PROJECT_NAME}_BUILD_GRAPHICS "If ON, Monocle will build the Graphics library." ON)
option(${PROJECT_NAME}_BUILD_GAMEFRAMEWORK "If ON, Monocle will build the GameFramework library." ON)
option(${PROJECT_NAME}_BUILD_MATH "If ON, Monocle will build the Math library." ON)
option(${PROJECT_NAME}_BUILD_PHYSICS "If ON, Monocle will build the Physics library." ON)

# Monocle support projects to build
option(${PROJECT_NAME}_BUILD_UNIT_TESTS "If ON, Monocle will build the Unit Tests project." OFF)
option(${PROJECT_NAME}_BUILD_SANDBOX "If ON, Monocle will build a test project." ON)


# End of options

# C++17 features used :
# std::optional, [[nodiscard]], structured bindings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Definitions tweaking (I think this should rather be in a configure file...)

add_compile_definitions($<$<CONFIG:Debug>:MOE_DEBUG>)

if(${PROJECT_NAME}_USE_ASSERTS)
	add_definitions(-DMOE_USE_ASSERTS)
endif()
if(${PROJECT_NAME}_BUILD_SHARED_LIBS)
	add_definitions(-DMOE_USE_DLL)
endif()
if(${PROJECT_NAME}_USE_STL)
	add_definitions(-DMOE_STD_SUPPORT)
endif()
if(${PROJECT_NAME}_USE_WIN32)
	add_definitions(-DMOE_USE_WIN32)
endif()
if(${PROJECT_NAME}_USE_GLFW)
	add_definitions(-DMOE_GLFW)
endif()

if(${PROJECT_NAME}_USE_GLM)
	add_definitions(-DMOE_GLM)
	
	# Add GLM library project
	set(GLM_DIR "${VENDOR_DIR}/glm")
	file(GLOB_RECURSE GLM_FILES "${GLM_DIR}/*.hpp" "${GLM_DIR}/*.inl")
	add_library(GLM INTERFACE) # GLM is header-only: make it an interface
	#add_custom_target(GLM_ SOURCES ${GLM_FILES})
	target_sources(GLM INTERFACE "${GLM_FILES}")
	#set_target_properties(GLM PROPERTIES LINKER_LANGUAGE CXX) # Fixes "can not determine linker language" CMake error 
	target_include_directories(GLM INTERFACE "${GLM_DIR}")
endif()

if(${PROJECT_NAME}_USE_OPENGL)
	add_definitions(-DMOE_OPENGL)
	
	# Add GLAD project
	set(GLAD_DIR "${VENDOR_DIR}/glad45")
	set(GLAD_SOURCE "${GLAD_DIR}/src/glad.c")
	
	# Do not recompile Glad when rebuilding ALL
	add_library(GLAD EXCLUDE_FROM_ALL "${GLAD_SOURCE}")
	
	# Make the glad.c file a "C++ file" or it just won't be compiled (because the project is marked C++)
	set_source_files_properties( "${GLAD_SOURCE}" PROPERTIES LANGUAGE CXX )
	
	target_include_directories(GLAD PUBLIC "${GLAD_DIR}/include")
endif()


# Integrate STB
if(${PROJECT_NAME}_USE_STB_IMAGE_IMPORTER)
	set(STB_DIR "${VENDOR_DIR}/stb")
endif()


if (WIN32)
	add_definitions(-DMOE_WINDOWS)
elseif(APPLE)
	add_definitions(-DMOE_MACOS)
elseif(UNIX)
	add_definitions(-DMOE_LINUX)
endif()





# Definitions tweaking End

# Creating an Interface Library in order to share PCH files with all built libraries.
# This way I can define a list of headers inside a pch without adding them to any "actual" target.
add_library(${PROJECT_NAME} INTERFACE)
# Add here all the headers that should be in precompiled header file.
if(${PROJECT_NAME}_USE_STL)
	target_precompile_headers(${PROJECT_NAME}
	INTERFACE
		"${SOURCE_DIR}/pch.h"
	INTERFACE
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
  INTERFACE
    "${SOURCE_DIR}/pch.h"
  INTERFACE
)
endif()

# Include our utility functions for all submodules to have them too
include(CMake/MonocleFunctions.cmake)

# Put all our generated binaries and DLLs in the same folder so that built executables find the library DLLs.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the appropriate lib type (courtesy of https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/BuildingWinDLL)
set(MONOCLE_LIB_TYPE STATIC)
if(${PROJECT_NAME}_BUILD_SHARED_LIBS)
  set(MONOCLE_LIB_TYPE SHARED)
endif()

if(${PROJECT_NAME}_BUILD_CORE)
	add_subdirectory(${SOURCE_DIR}/Core)
endif()

if(${PROJECT_NAME}_BUILD_INPUT)
	add_subdirectory(${SOURCE_DIR}/Input)
endif()

if(${PROJECT_NAME}_BUILD_APPLICATION)
	add_subdirectory(${SOURCE_DIR}/Application)
endif()

if(${PROJECT_NAME}_BUILD_GRAPHICS)
	add_subdirectory("${SOURCE_DIR}/Graphics")
endif()

if(${PROJECT_NAME}_BUILD_GAMEFRAMEWORK)
	add_subdirectory("${SOURCE_DIR}/GameFramework")
endif()

if(${PROJECT_NAME}_BUILD_MATH)
	add_subdirectory("${SOURCE_DIR}/Math")
endif()

# if(${PROJECT_NAME}_BUILD_PHYSICS)
	# add_subdirectory("${SOURCE_DIR}/Physics")
# endif()

if(${PROJECT_NAME}_BUILD_UNIT_TESTS)
	add_subdirectory(UnitTests)
endif()

if(${PROJECT_NAME}_BUILD_SANDBOX)
	add_subdirectory(Sandbox)
endif()

# I think adding the CMakeLists in glm helps the natvis to work ?...
# Fixed a hard to repro natvis bug because of that.
if(${PROJECT_NAME}_USE_GLM)
	add_subdirectory("${GLM_DIR}")
endif()
