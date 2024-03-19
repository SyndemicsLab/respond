cmake_minimum_required(VERSION 3.20)
project(respondpy CXX)

# Find python and Boost - both are required dependencies
find_package(PythonLibs REQUIRED)
find_package(Boost COMPONENTS python310 REQUIRED)

set(CMAKE_SHARED_MODULE_PREFIX "")

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(Matrixify SHARED Matrixify_ext.cpp)
add_library(Simulation SHARED Simrunner_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(Matrixify PUBLIC ${Boost_LIBRARIES})
target_link_libraries(Matrixify PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(Matrixify PUBLIC simrunner)
target_link_libraries(Matrixify PUBLIC matrixify)

target_link_libraries(Simulation PUBLIC ${Boost_LIBRARIES})
target_link_libraries(Simulation PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(Simulation PUBLIC simrunner)
target_link_libraries(Simulation PUBLIC matrixify)

target_include_directories(Matrixify PRIVATE ${PYTHON_INCLUDE_DIRS})
target_include_directories(Simulation PRIVATE ${PYTHON_INCLUDE_DIRS})