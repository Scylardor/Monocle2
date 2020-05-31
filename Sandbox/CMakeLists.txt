cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(SANDBOX_TARGET ${PROJECT_NAME}_Sandbox)
set(SOURCE_DIR source)

set(${SANDBOX_TARGET}_SOURCES
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/TestApplication.cpp"
	"${SOURCE_DIR}/Basics.cpp"
	"${SOURCE_DIR}/Advanced1.cpp"
	"${SOURCE_DIR}/TestApplication.h"
)

add_compile_options(
    "$<$<CONFIG:Debug>:-DMOE_DEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:-DMOE_DEBUG>"
)

add_executable(${SANDBOX_TARGET} ${${SANDBOX_TARGET}_SOURCES} )

target_include_directories(${SANDBOX_TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/source
)

target_link_libraries(${SANDBOX_TARGET} PRIVATE
	GLM
	${PROJECT_NAME}_Core
	${PROJECT_NAME}_Application
	${PROJECT_NAME}_Math
	${PROJECT_NAME}_Graphics
	${PROJECT_NAME})  # Linking with project's Interface Library allows us to reuse PCH's.


# Setup boilerplate Monocle target configurations (warning level, LTCG etc.)
monocle_target_compile_config(${SANDBOX_TARGET})
