cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# # Find OpenMP
# find_package(OpenMP REQUIRED)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(fmt REQUIRED)

# Find Boost
find_package(Boost REQUIRED COMPONENTS log_setup log)

add_library(model
    Simulation.cpp
    PostSimulationCalculator.cpp
)

target_include_directories(model INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  "include/"
)

target_link_libraries(model
  ${Boost_LIBRARIES}
  fmt::fmt
  Eigen3::Eigen
  data
  utilities
)
