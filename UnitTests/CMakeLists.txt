cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(UNIT_TESTS_TARGET ${PROJECT_NAME}_UnitTests)
set(SOURCE_DIR source)

set(${UNIT_TESTS_TARGET}_SOURCES
	"${SOURCE_DIR}/TestContainers.cpp"
	"${SOURCE_DIR}/TestDelegates.cpp"
	"${SOURCE_DIR}/TestFSM.cpp"
	"${SOURCE_DIR}/TestHashString.cpp"
	"${SOURCE_DIR}/TestInput.cpp"
	"${SOURCE_DIR}/TestLog.cpp"
	"${SOURCE_DIR}/Testmain.cpp"
	"${SOURCE_DIR}/TestStringFormat.cpp"
)


add_executable(${UNIT_TESTS_TARGET} ${${UNIT_TESTS_TARGET}_SOURCES} )

target_include_directories(${UNIT_TESTS_TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/vendor/Catch
	${PROJECT_SOURCE_DIR}/vendor
)

target_link_libraries(${UNIT_TESTS_TARGET} ${PROJECT_NAME} )

# use the main target's precompiled header.
target_precompile_headers(${UNIT_TESTS_TARGET} REUSE_FROM ${PROJECT_NAME})