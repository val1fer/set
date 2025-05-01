cmake_minimum_required(VERSION 3.20)
project(
    Set
    VERSION 1.0
    DESCRIPTION "STL compatible set"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(set INTERFACE include/set.hpp)
target_include_directories(set INTERFACE include)

add_subdirectory(example)

enable_testing()
add_subdirectory(tests)