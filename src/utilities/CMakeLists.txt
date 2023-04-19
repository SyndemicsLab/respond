cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_library(utilities
    Matrix3dFactory.cpp
)

target_sources(utilities PUBLIC 
    Matrix3dFactory.hpp
)

target_include_directories(utilities INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(utilities 
  Eigen3::Eigen
  data
)
