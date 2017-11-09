cmake_minimum_required( VERSION 3.2 )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/ )

include(CMakeParseArguments)
message(STATUS ${CMAKE_MODULE_PATH})
include(assert_build_fails)

#########################
# function definitions  #
#########################

# add_markdown
# ------------
#
# Add a markdown target that compiles into an html file.
#
# Parameters:
# OPT_TARGET     -- the target being defined (relative to binary dir)
# OPT_SOURCE     -- the main source file (relative to source dir)
# OPT_STYLESHEET -- the html stylesheet (css) that will be used to render the
#                   html document. (relative to source dir)
#
# Other dependencies:
# MD -- the markdown compiler script
#       (has to accept the three arguments in SOURCE, STYLESHEET, TARGET order)
#
set( MD "${CMAKE_SOURCE_DIR}/build_markdown.sh" )
function(add_markdown OPT_TARGET OPT_SOURCE OPT_STYLESHEET)
  set(TARGET "${CMAKE_CURRENT_BINARY_DIR}/${OPT_TARGET}")
  set(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${OPT_SOURCE}")
  set(STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/${OPT_STYLESHEET}")

  add_custom_command(
    OUTPUT "${TARGET}"
    COMMAND "${MD}" "${SOURCE}" "${STYLESHEET}" "${TARGET}"
    MAIN_DEPENDENCY "${SOURCE}"
    DEPENDS "${STYLESHEET}" "${MD}"
    VERBATIM)
endfunction(add_markdown)
##############################
#  end function definitions  #
##############################

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
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
