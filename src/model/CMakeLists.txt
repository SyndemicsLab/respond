cmake_minimum_required(VERSION 3.20)

find_package (Eigen3 3.4 REQUIRED)

add_library(model
    Simulation.cpp
    PostSimulationCalculator.cpp
)

target_include_directories(model INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  "${CMAKE_SOURCE_DIR}/src/data/include/"
  "${CMAKE_SOURCE_DIR}/src/utilities/include/"
)

target_link_libraries(model
  Eigen3::Eigen
  data
  utilities
)

install(TARGETS model
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
