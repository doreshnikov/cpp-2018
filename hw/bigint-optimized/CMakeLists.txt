cmake_minimum_required(VERSION 2.8)

project(bigint-optimized)

file(GLOB GTEST ../gtest/gtest.h ../gtest/*.cc)
file(GLOB BIGINTOPT *.h *.cpp)

add_executable(bigint_optimized_test
        ${GTEST}
        ${BIGINTOPT}
        ../big_integer_testing.cpp)

target_link_libraries(bigint_optimized_test -lpthread)
target_compile_definitions(bigint_optimized_test PRIVATE BIGINT_OPTIMIZED)