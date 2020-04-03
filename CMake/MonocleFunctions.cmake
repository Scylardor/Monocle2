function(MONOCLE_TARGET_COMPILE_CONFIG TARGET_NAME)
	# Optional IPO. Do not use IPO if it's not supported by compiler.
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result OUTPUT output)
	if(result)
		set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
	else()
		message(WARNING "IPO is not supported: ${output}")
	endif()
	
	# Enable Warning level to reasonable maximum and warning as errors.
	if(MSVC) # /WAll on MSVC is actually worse than /W4 and produces lots of unnecessary warnings we don't care about !
		target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX) 
	else()
		target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
	endif()
	
	# Include auto-source regeneration script for this target if there is one
	set(AUTO_REGEN_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/_scripts/generate_cmake_sources.sh")
	
	if(EXISTS ${AUTO_REGEN_SCRIPT})
		target_sources(${TARGET_NAME} PRIVATE ${AUTO_REGEN_SCRIPT})
	endif()
endfunction()


function(MONOCLE_INCLUDE_GLFW3 TARGET_NAME)
	# Point to our CMake dir with some code to find external dependencies
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake/")
	
	# Hint at our supposed GLFW3 installation location
	set(GLFW3_ROOT ${PROJECT_SOURCE_DIR}/vendor/glfw3)
	find_package(GLFW3 REQUIRED)
	message(STATUS "Found GLFW3 include dir: ${GLFW3_INCLUDE_DIR}")
	message(STATUS "Found GLFW3 library: ${GLFW3_LIBRARY}")
	
	# The GLFW_INCLUDE_NONE define will make GLFW not include any graphics library-related files,
	# and helps fixing a "disagreement" with GLAD because GLAD always wants to be included first.
	target_compile_definitions(${TARGET_NAME} PRIVATE "GLFW_INCLUDE_NONE")
	target_include_directories(${TARGET_NAME} PRIVATE ${GLFW3_INCLUDE_DIR})
	target_link_libraries(${TARGET_NAME} PRIVATE ${GLFW3_LIBRARY})
	
	# We expect the DLL file to be next to the import library file.
	# Next, we copy the DLL next to the executables.
	string(REPLACE ".lib" ".dll" GLFW_DLL ${GLFW3_LIBRARY})
 
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different  
		${GLFW_DLL}
		$<TARGET_FILE_DIR:${TARGET_NAME}>)

endfunction()