cmake_minimum_required(VERSION 3.21)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

# Find python and Boost - both are required dependencies
set(PYBIND11_NEWPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
# add_library(data_ops SHARED data_ops_ext.cpp)
# add_library(Simulation SHARED Simrunner_ext.cpp)

pybind11_add_module(data_ops MODULE respond.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(data_ops 
PUBLIC
    ${PYTHON_LIBRARIES}
    datamanagement::datamanagement
    respond
PRIVATE 
    pybind11::headers
)

# SET_TARGET_PROPERTIES(Simulation PROPERTIES SUFFIX .pyd)

install(TARGETS respond RUNTIME_DEPENDENCIES LIBRARY DESTINATION .)
install(IMPORTED_RUNTIME_ARTIFACTS respond DESTINATION .)
