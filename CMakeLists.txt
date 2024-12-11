cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(synmodels)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

include(LoadEigen)
LoadEigen()

add_library(${PROJECT_NAME} STATIC src/synmodels.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(src)
# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()

if(BUILD_PYBINDINGS)
    include(LoadPybind)
    LoadPybind()
    add_subdirectory(extras/py_synmodels)
endif()

# Only compile benchmarking if desired
option(BUILD_BENCHMARK "enable RESPOND benchmarking" OFF)
if (NOT BUILD_BENCHMARK STREQUAL OFF)
  add_subdirectory(extras/benchmarking)
endif()
