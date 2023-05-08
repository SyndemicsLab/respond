cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# Link Boost
find_package(Boost REQUIRED COMPONENTS
  program_options
)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find FMT
find_package (fmt REQUIRED)

add_library(data
  Loader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  DataWriter.cpp
  DataTypes.cpp
  Configuration.cpp
)
target_sources(data PUBLIC
  Loader.hpp
  CostLoader.hpp
  UtilityLoader.hpp
  DataLoader.hpp
  DataWriter.hpp
  DataTypes.hpp
)
target_include_directories(data INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(data
  ${Boost_LIBRARIES}
  model
  fmt::fmt
  Eigen3::Eigen
)
