cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (
  RESPOND
  VERSION 2.0.0
  DESCRIPTION "A Compartmental Simulation for Opioid Use Disorder"
)

include(GNUInstallDirs)

find_package(Eigen3 3.4 REQUIRED)

add_subdirectory(model)
add_subdirectory(data)
add_subdirectory(utilities)

add_executable(respond main.cpp)

target_link_libraries(respond
  data
  model
)

# Create a simple configuration header
configure_file(config.h.in config.h)
