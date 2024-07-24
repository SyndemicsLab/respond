cmake_minimum_required(VERSION 3.20)
project(globalRESPOND)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_subdirectory(lib/DataManagement)
add_subdirectory(src)
# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(test)
endif()

# Only compile benchmarking if desired
option(BUILD_BENCHMARK "enable RESPOND benchmarking" OFF)
if (NOT BUILD_BENCHMARK STREQUAL OFF)
  add_subdirectory(benchmark)
endif()

# Only build the API if desired
if(BUILD_API STREQUAL ON)
  add_subdirectory(lib/Crow)
endif()
