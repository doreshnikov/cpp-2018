cmake_minimum_required(VERSION 3.8)

project(bigint)

file(GLOB BIGINT *.h *.cpp)

add_executable(bigint_test
        ${BIGINT}
        ../big_integer_testing.cpp)

target_link_libraries(bigint_test gtest -lpthread)