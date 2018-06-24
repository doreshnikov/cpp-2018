cmake_minimum_required(VERSION 2.8)

project(bigint)

file(GLOB GTEST ../gtest/gtest.h ../gtest/*.cc)
file(GLOB BIGINT *.h *.cpp)

add_executable(bigint_test
        ${GTEST}
        ${BIGINT}
        ../big_integer_testing.cpp)

target_link_libraries(bigint_test -lpthread)