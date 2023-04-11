cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# Find OpenMP
find_package(OpenMP REQUIRED)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find Boost
find_package(Boost REQUIRED COMPONENTS log_setup log)

include(FetchContent)
# Find FMT
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_library(Model
    Simulation.cpp 
)

target_sources(Model PUBLIC 
  Simulation.hpp
)

target_include_directories(Model INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Model 
  OpenMP::OpenMP_CXX 
  ${Boost_LIBRARIES}
  fmt::fmt
  Eigen3::Eigen
  Controller
)
