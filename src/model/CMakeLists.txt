cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# # Find OpenMP
# find_package(OpenMP REQUIRED)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

# Find fmt
find_package(fmt REQUIRED)

# Find Boost
find_package(Boost REQUIRED COMPONENTS log_setup log)

add_library(model
    Simulation.cpp
    CostCalculator.cpp
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
