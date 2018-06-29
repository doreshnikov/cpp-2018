cmake_minimum_required(VERSION 3.8)

project(circular-buffer)

add_library(counted counted.h counted.cpp fault_injection.h fault_injection.cpp)

add_executable(std
        std.cpp)

add_executable(circular_buffer_testing
        circular_buffer_testing.cpp)

target_link_libraries(std gtest counted -lpthread)
target_link_libraries(circular_buffer_testing gtest counted -lpthread)