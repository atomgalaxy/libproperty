cmake_minimum_required( VERSION 3.2 )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/ )

include(CMakeParseArguments)
message(STATUS ${CMAKE_MODULE_PATH})
include(assert_build_fails)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(libproperty)

enable_testing()

# enable c++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g ")

include_directories("${CMAKE_SOURCE_DIR}")
add_executable(examples ./tests/examples.cpp)
add_test(NAME examples COMMAND examples)

add_executable(conformance ./tests/conformance.cpp)
add_test(NAME conformance COMMAND conformance)

# movable test & negatives
#assert_build_fails(TEST_NAME negative_test_copyable
#                   TARGET negative_test_copyable test/test_type_erasure_movable.cpp
#                   DEFINITIONS NOCOMPILE_COPYABLE_TEST
#                   )
#assert_build_fails(TEST_NAME negative_test_movable
#                   TARGET negative_test_movable test/test_type_erasure_movable.cpp
#                   DEFINITIONS NOCOMPILE_MOVABLE_TEST
#                   )
