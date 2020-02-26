cmake_minimum_required (VERSION 2.6)

project (reactive-framework-cpp)

include_directories("/usr/src/gtest/include/gtest/")
link_directories("$ENV{HOME}/install/gtest/build")
find_package(GTest)

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} gtest pthread)
gtest_discover_tests(${CMAKE_PROJECT_NAME})
