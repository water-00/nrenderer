
enable_language(C CXX)

file(GLOB_RECURSE IMGUI_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE IMGUI_HEADER_FILES "./include/*.h ./include/*.hpp")


source_group("Header Files" FILES ${IMGUI_HEADER_FILES})

add_library(imgui STATIC "${IMGUI_SOURCE_FILES}" "${IMGUI_HEADER_FILES}")
target_include_directories(imgui PRIVATE "./include")

