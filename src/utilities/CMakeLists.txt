cmake_minimum_required(VERSION 3.20)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

add_library(utilities
    Matrix3dFactory.cpp
    Matrix3dPrinter.cpp
)

target_include_directories(utilities PRIVATE
  "include/"
)

target_link_libraries(utilities 
  Eigen3::Eigen
  # data
)

install(TARGETS utilities
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
