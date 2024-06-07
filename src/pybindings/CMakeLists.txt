cmake_minimum_required(VERSION 3.20)
project(respondpy CXX)

# Find python and Boost - both are required dependencies
find_package(Python 3.11 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
# add_library(Matrixify SHARED Matrixify_ext.cpp)
# add_library(Simulation SHARED Simrunner_ext.cpp)

pybind11_add_module(Matrixify MODULE Matrixify_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(Matrixify PRIVATE pybind11::headers)
#target_link_libraries(Matrixify PUBLIC ${Boost_LIBRARIES})
target_link_libraries(Matrixify PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(Matrixify PUBLIC simrunner)
target_link_libraries(Matrixify PUBLIC matrixify)

# SET_TARGET_PROPERTIES(Matrixify PROPERTIES SUFFIX .pyd)

pybind11_add_module(Simulation MODULE Simrunner_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(Simulation PRIVATE pybind11::headers)
#target_link_libraries(Simulation PUBLIC ${Boost_LIBRARIES})
target_link_libraries(Simulation PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(Simulation PUBLIC simrunner)
target_link_libraries(Simulation PUBLIC matrixify)

# SET_TARGET_PROPERTIES(Simulation PROPERTIES SUFFIX .pyd)
