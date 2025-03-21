cmake_minimum_required(VERSION 3.20)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${SKBUILD_PROJECT_NAME} respond.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(${SKBUILD_PROJECT_NAME} 
PRIVATE
    respond
)

install(TARGETS ${SKBUILD_PROJECT_NAME} LIBRARY DESTINATION .)
