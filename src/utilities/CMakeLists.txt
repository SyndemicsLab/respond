cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_library(utilities
    Matrix3dFactory.cpp
    Matrix3dPrinter.cpp
)

target_include_directories(utilities INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  "include/"
)

target_link_libraries(utilities 
  Eigen3::Eigen
  data
)
