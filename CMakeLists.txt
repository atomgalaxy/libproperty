cmake_minimum_required( VERSION 3.2 )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/ )

include(CMakeParseArguments)
message(STATUS ${CMAKE_MODULE_PATH})
include(assert_build_fails)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(libproperty)

enable_testing()

# enable c++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_SOURCE_DIR}")

add_executable(examples ./tests/examples.cpp)
add_test(NAME examples COMMAND examples)

add_executable(readme_example ./tests/readme_example.cpp)
add_test(NAME readme_example COMMAND readme_example)

add_executable(short_example ./tests/short_example.cpp)
add_test(NAME short_example COMMAND short_example)

add_executable(conformance ./tests/conformance.cpp)
add_test(NAME conformance COMMAND conformance)

add_executable(move ./tests/move.cpp)
add_test(NAME move COMMAND move)

add_executable(test-wrapper ./tests/test-wrapper.cpp)
add_test(NAME test-wrapper COMMAND test-wrapper)

add_executable(talk ./tests/talk.cpp)
add_test(NAME talk COMMAND talk)

# movable test & negatives
#assert_build_fails(TEST_NAME negative_test_copyable
#                   TARGET negative_test_copyable test/test_type_erasure_movable.cpp
#                   DEFINITIONS NOCOMPILE_COPYABLE_TEST
#                   )
assert_build_fails(TEST_NAME negative_test_movable
                   TARGET negative_test_movable tests/move.cpp
                   DEFINITIONS ENSURE_DOES_NOT_COMPILE_1
                   )
