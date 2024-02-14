cmake_minimum_required(VERSION 3.20)
project(globalRESPOND)

add_subdirectory(src)
# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()