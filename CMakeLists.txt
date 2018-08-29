
project(aggregate-vector)
cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

enable_testing()
add_executable(tests "tests/tests.cpp")
add_test(NAME tests COMMAND tests)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(tests PUBLIC "/W4 /WarnAsError")
else ()
    target_compile_options(tests PUBLIC "-std=c++17 -Wall -Wextra -Werror")
endif()
