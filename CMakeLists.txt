cmake_minimum_required(VERSION 3.20)

project(synmodels)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(src)
# Only compile tests if desired
option(BUILD_TESTS "enable RESPOND tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()

if(BUILD_PYBINDINGS)
    message(CHECK_START "Fetching Pybind11")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        v2.13.6
    )
    FetchContent_MakeAvailable(pybind11)
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "fetched")
    pybind11_add_module(py_synmodels src/py_SynModels.cpp)
    target_link_libraries(py_synmodels
        PRIVATE
            synmodels
            data
            kernels
            models
            Eigen3::Eigen
    )
endif()

# Only compile benchmarking if desired
# option(BUILD_BENCHMARK "enable RESPOND benchmarking" OFF)
# if (NOT BUILD_BENCHMARK STREQUAL OFF)
#   add_subdirectory(benchmark)
# endif()

# Only build the API if desired
# if(BUILD_API STREQUAL ON)
#   add_subdirectory(lib/Crow)
# endif()
