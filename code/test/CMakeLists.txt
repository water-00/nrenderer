cmake_minimum_required(VERSION 3.18)

enable_testing()

message("Google Test Dir: ${gtest_SOURCE_DIR}")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

file(GLOB_RECURSE TEST_SOURCE_FILES "./*.cpp")
add_executable(NR_GTest "${TEST_SOURCE_FILES}")

target_link_libraries(NR_GTest gtest gtest_main NRServer)

add_test(NR_GTest NR_GTest)