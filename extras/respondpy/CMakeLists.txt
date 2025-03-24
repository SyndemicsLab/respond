cmake_minimum_required(VERSION 3.24)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${SKBUILD_PROJECT_NAME} respondpy.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(${SKBUILD_PROJECT_NAME} 
PRIVATE
    respond
    pybind11::module
)

pybind11_extension(${SKBUILD_PROJECT_NAME})
if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    # Strip unnecessary sections of the binary on Linux/macOS
    pybind11_strip(${SKBUILD_PROJECT_NAME})
endif()

install(TARGETS ${SKBUILD_PROJECT_NAME} LIBRARY DESTINATION .)
