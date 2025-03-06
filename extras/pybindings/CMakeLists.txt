cmake_minimum_required(VERSION 3.21)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

# Find python and Boost - both are required dependencies
set(PYBIND11_NEWPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
# add_library(data_ops SHARED data_ops_ext.cpp)
# add_library(Simulation SHARED Simrunner_ext.cpp)

pybind11_add_module(data_ops MODULE data_ops_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(data_ops PRIVATE pybind11::headers)
target_link_libraries(data_ops PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(data_ops PUBLIC simrunner)
target_link_libraries(data_ops PUBLIC matrixify)
target_link_libraries(data_ops PUBLIC DataManagement)

# SET_TARGET_PROPERTIES(data_ops PROPERTIES SUFFIX .pyd)

pybind11_add_module(Simulation MODULE Simrunner_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(Simulation PRIVATE pybind11::headers)
target_link_libraries(Simulation PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(Simulation PUBLIC simrunner)
target_link_libraries(Simulation PUBLIC matrixify)
target_link_libraries(data_ops PUBLIC DataManagement)

# SET_TARGET_PROPERTIES(Simulation PROPERTIES SUFFIX .pyd)

install(TARGETS data_ops RUNTIME_DEPENDENCIES LIBRARY DESTINATION .)
install(IMPORTED_RUNTIME_ARTIFACTS data_ops DESTINATION .)
install(TARGETS Simulation RUNTIME_DEPENDENCIES LIBRARY DESTINATION .)
install(IMPORTED_RUNTIME_ARTIFACTS Simulation DESTINATION .)
