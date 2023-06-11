cmake_minimum_required(VERSION 3.18)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/components)

# add your component directory
add_subdirectory("./example")
add_subdirectory("./simple_path_tracing")
add_subdirectory("./ray_cast")
add_subdirectory("./acc_path_tracing")
add_subdirectory("./metropolis_light_transport")