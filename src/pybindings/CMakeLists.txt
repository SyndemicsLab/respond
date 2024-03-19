cmake_minimum_required(VERSION 3.20)
project(respondpy CXX)

# Find python and Boost - both are required dependencies
find_package(PythonLibs REQUIRED)

find_package(Boost COMPONENTS python310 REQUIRED)

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(${PROJECT_NAME} SHARED Matrixify_ext.cpp Simrunner_ext.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PYTHON_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC simrunner)
target_link_libraries(${PROJECT_NAME} PUBLIC matrixify)

target_include_directories(${PROJECT_NAME} PRIVATE ${PYTHON_INCLUDE_DIRS})