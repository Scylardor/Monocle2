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
	"${SOURCE_DIR}/TestMath.cpp"
	"${SOURCE_DIR}/TestStringFormat.cpp"
	"${SOURCE_DIR}/TestGraphicsBuddyAllocator.cpp"
	"${SOURCE_DIR}/TestStopwatch.cpp"
)

add_executable(${UNIT_TESTS_TARGET} EXCLUDE_FROM_ALL ${${UNIT_TESTS_TARGET}_SOURCES} )

target_compile_definitions(${UNIT_TESTS_TARGET} PUBLIC MOE_UNIT_TESTS)

target_include_directories(${UNIT_TESTS_TARGET} PRIVATE
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/vendor/Catch
	${PROJECT_SOURCE_DIR}/vendor
)

target_link_libraries(${UNIT_TESTS_TARGET}
	PUBLIC ${PROJECT_NAME}_Application
	PUBLIC ${PROJECT_NAME}_Graphics
	PUBLIC ${PROJECT_NAME}_GameFramework
	PUBLIC ${PROJECT_NAME}_Input
	PUBLIC ${PROJECT_NAME}_Math
	PRIVATE ${PROJECT_NAME} )  # Linking with project's Interface Library allows us to reuse PCH's.

# Setup boilerplate Monocle target configurations (warning level, LTCG etc.)
monocle_target_compile_config(${UNIT_TESTS_TARGET})