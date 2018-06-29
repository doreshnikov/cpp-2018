cmake_minimum_required(VERSION 3.8)

project(bigint-optimized)

file(GLOB BIGINTOPT *.h *.cpp)

add_executable(bigint_optimized_test
        ${BIGINTOPT}
        ../big_integer_testing.cpp)

target_link_libraries(bigint_optimized_test gtest -lpthread)
target_compile_definitions(bigint_optimized_test PRIVATE BIGINT_OPTIMIZED)