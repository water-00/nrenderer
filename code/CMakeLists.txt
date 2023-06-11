# nku 1711326

cmake_minimum_required(VERSION 3.18)

project(NRenderer VERSION 2.0
		DESCRIPTION	"服务于本科教学的三维渲染系统"
		LANGUAGES C CXX)

message(${PROJECT_SOURCE_DIR})
message(${PROJECT_NAME})
message("BUILD_RPATH TYPE: " ${CMAKE_BUILD_TYPE})

set(SERVER_HEADER_DIR "${PROJECT_SOURCE_DIR}/include")
set(SERVER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/server")
set(DEPENDENCES_DIR "${PROJECT_SOURCE_DIR}/dependences")
set(COMPONENTS_DIR "${PROJECT_SOURCE_DIR}/components")
set(APP_DIR "${PROJECT_SOURCE_DIR}/app")

# Dependences include and ...
include_directories(
	"${SERVER_HEADER_DIR}"
	"${DEPENDENCES_DIR}/glad/include"
	"${DEPENDENCES_DIR}/glfw3/include"
	"${DEPENDENCES_DIR}/imgui/include"
	"${DEPENDENCES_DIR}/glm"
	"${DEPENDENCES_DIR}/stb_image/include"
)


if (CMAKE_GENERATOR STREQUAL "Ninja")
	message("MSVC......")
	if (MSVC_VERSION GREATER_EQUAL "1900")
		include(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
			if (_cpp_latest_flag_supported)
				add_compile_options("/std:c++latest")
			endif()
			if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
				add_compile_options("/Ox")
			elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
				add_compile_options("/Os")
			endif()
	endif()
endif()

# Server
file(GLOB_RECURSE SERVER_HEADER_FILES "${SERVER_HEADER_DIR}/*.h" "${SERVER_HEADER_DIR}/*.hpp")
source_group("Header Files" FILES ${SERVER_HEADER_FILES})
file(GLOB_RECURSE SERVER_SOURCE_FILES "${SERVER_SOURCE_DIR}/*.cpp")
add_library(NRServer SHARED "${SERVER_SOURCE_FILES}" "${SERVER_HEADER_FILES}" "components/acc_path_tracing/include/Bounds3.hpp" "components/acc_path_tracing/include/BVH.hpp")

# Src

# UI
add_subdirectory(${APP_DIR})

# Dependences
add_subdirectory("${DEPENDENCES_DIR}/glad")
add_subdirectory("${DEPENDENCES_DIR}/imgui")

# Components

# Main
add_executable(${PROJECT_NAME} main.cpp "components/acc_path_tracing/include/Bounds3.hpp" "components/acc_path_tracing/include/BVH.hpp")
target_link_libraries(${PROJECT_NAME} NRApp)
target_include_directories(${PROJECT_NAME} PRIVATE "./app/include")

# Google Test
add_subdirectory("${DEPENDENCES_DIR}/gtest")
add_subdirectory(test)

# Components
add_subdirectory("${COMPONENTS_DIR}")
