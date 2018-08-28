
project(aggregate-vector)
cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)

enable_testing()
add_executable(tests "tests.cpp")
add_test(NAME tests COMMAND tests)
