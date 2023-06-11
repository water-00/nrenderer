enable_language(C CXX)

set(GLAD_SRC "./src/glad.c")

file(GLOB_RECURSE GLAD_HEADER_FILES "./include/*.h ./include/*.hpp")
source_group("Header Files" FILES ${GLAD_HEADER_FILES})

add_library(glad STATIC "${GLAD_SRC}" "${GLAD_HEADER_FILES}")
target_include_directories(glad PRIVATE "./include")