cmake_minimum_required(VERSION 3.17)
project(ia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ia src/indexed_array.cpp)
add_executable(var src/variadic.cpp)
add_executable(ba src/tuple_hash.hpp src/bin_arr.cpp)
add_executable(ht src/tuple_hash.hpp src/hash_tuple.cpp)
