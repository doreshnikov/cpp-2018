cmake_minimum_required(VERSION 2.8)

project(circular-buffer)

file(GLOB GTEST ../hw/gtest/gtest.h ../hw/gtest/*.cc)

add_Executable(circular_buffer_testing
        ${GTEST}
        circular_buffer.h
        fault_injection.h fault_injection.cpp
        counted.h counted.cpp
        circular_buffer_testing.cpp)

target_link_libraries(circular_buffer_testing -lpthread)