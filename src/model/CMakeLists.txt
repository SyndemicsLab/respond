cmake_minimum_required(VERSION 3.20)

find_package (Eigen3 3.4 REQUIRED)

add_library(model
    Simulation.cpp
    PostSimulationCalculator.cpp
)

target_include_directories(model PRIVATE
  "include/"
  "../data/include/"
)

target_link_libraries(model
  Eigen3::Eigen
  data
)
