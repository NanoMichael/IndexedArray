cmake_minimum_required(VERSION 3.17)
project(ia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src/hash_tuple.h)

add_executable(ia src/indexed_array.cpp)
add_executable(var src/variadic.cpp)
add_executable(ba src/bin_arr.cpp)
add_executable(ht src/hash_tuple.cpp)
