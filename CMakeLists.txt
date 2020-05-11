cmake_minimum_required(VERSION 3.17)
project(ia)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ia src/indexed_array.cpp)
add_executable(var src/variadic.cpp)
add_executable(ba src/bin_arr.cpp)
