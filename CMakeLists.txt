cmake_minimum_required(VERSION 3.20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(synmodels_root)

include(LoadEigen)
LoadEigen()
add_subdirectory(src)

# Only compile tests if desired
if (BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_PYBINDINGS)
    include(LoadPybind)
    LoadPybind()
    add_subdirectory(extras/py_synmodels)
endif()

if (BUILD_BENCHMARK)
  add_subdirectory(extras/benchmarking)
endif()
