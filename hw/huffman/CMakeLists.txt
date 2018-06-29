cmake_minimum_required(VERSION 3.8)

project(huffman)

file(GLOB HUFFMAN_LIB lib/*.h lib/*.cpp)
file(GLOB HUFFMAN_EX tool/*.h tool/*.cpp)

add_executable(huffman
        ${HUFFMAN_LIB}
        ${HUFFMAN_EX})

add_executable(huffman_testing
        ${HUFFMAN_LIB}
        tool/buffered_reader.h tool/buffered_reader.cpp
        tool/buffered_writer.h tool/buffered_writer.cpp
        tests/huffman_lib_testing.cpp)

target_link_libraries(huffman_testing gtest -lpthread)