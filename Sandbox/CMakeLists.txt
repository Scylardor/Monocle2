cmake_minimum_required(VERSION 3.16) # 3.16 is needed for target_precompile_headers

set(SANDBOX_TARGET ${PROJECT_NAME}_Sandbox)
set(SOURCE_DIR source)

set(${SANDBOX_TARGET}_SOURCES
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/TestApplication.cpp"
	"${SOURCE_DIR}/TestVkApplication.cpp"
	"${SOURCE_DIR}/Basics.cpp"
	"${SOURCE_DIR}/Advanced1.cpp"
	"${SOURCE_DIR}/AdvancedLighting.cpp"
	"${SOURCE_DIR}/PBR.cpp"
	"${SOURCE_DIR}/TestApplication.h"
	"${SOURCE_DIR}/DirectionalShadowDepthPass.h"
	"${SOURCE_DIR}/DirectionalShadowDepthPass.cpp"
	"${SOURCE_DIR}/OmnidirectionalShadowDepthPass.h"
	"${SOURCE_DIR}/OmnidirectionalShadowDepthPass.cpp"
	"${SOURCE_DIR}/BasicQuad.h"
	"${SOURCE_DIR}/BasicQuad.cpp"
	"${SOURCE_DIR}/CubeWorld.h"
	"${SOURCE_DIR}/CubeWorld.cpp"
	"${SOURCE_DIR}/MutliViewportCubeWorld.h"
	"${SOURCE_DIR}/MutliViewportCubeWorld.cpp"
	"${SOURCE_DIR}/Phong.h"
	"${SOURCE_DIR}/Phong.cpp"
	"${SOURCE_DIR}/BlinnPhong.h"
	"${SOURCE_DIR}/BlinnPhong.cpp"
	"${SOURCE_DIR}/ImGUI.h"
	"${SOURCE_DIR}/ImGUI.cpp"
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
	${PROJECT_NAME}_GameFramework
	${PROJECT_NAME})  # Linking with project's Interface Library allows us to reuse PCH's.

set_property(TARGET ${SANDBOX_TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")


# Setup boilerplate Monocle target configurations (warning level, LTCG etc.)
monocle_target_compile_config(${SANDBOX_TARGET})
