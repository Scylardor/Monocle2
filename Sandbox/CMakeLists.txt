cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(SANDBOX_TARGET ${PROJECT_NAME}_Sandbox)
set(SOURCE_DIR source)

set(${SANDBOX_TARGET}_SOURCES
	"${SOURCE_DIR}/main.cpp"
)


add_executable(${SANDBOX_TARGET} ${${SANDBOX_TARGET}_SOURCES} )

target_include_directories(${SANDBOX_TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/vendor
)

target_link_libraries(${SANDBOX_TARGET} ${PROJECT_NAME} )

# use the main target's precompiled header.
target_precompile_headers(${SANDBOX_TARGET} REUSE_FROM ${PROJECT_NAME})