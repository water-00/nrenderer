cmake_minimum_required(VERSION 3.18)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE APP_HEADER_FILES "./*.h" "./*.hpp")
source_group("Header Files" FILES ${APP_HEADER_FILES})
file(GLOB_RECURSE APP_SOURCE_FILES "./*.cpp")
add_library(NRApp "${APP_SOURCE_FILES}" "${APP_HEADER_FILES}")

target_include_directories(NRApp PRIVATE "./include/")

# Dependences
target_link_libraries(NRApp glad)
target_link_libraries(NRApp "${DEPENDENCES_DIR}/glfw3/lib/glfw3.lib")
target_link_libraries(NRApp imgui)

# 
target_link_libraries(NRApp NRServer)