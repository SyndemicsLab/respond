cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Link Boost
find_package(Boost REQUIRED COMPONENTS
  program_options
)

# Find fmt
find_package(fmt REQUIRED)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

add_library(data
  Loader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  DataWriter.cpp
  DataTypes.cpp
  Configuration.cpp
)

target_include_directories(data INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
  "include/"
)
target_link_libraries(data
  ${Boost_LIBRARIES}
  model
  fmt::fmt
  Eigen3::Eigen
)
