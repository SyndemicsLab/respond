cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find FMT
find_package (fmt REQUIRED)

add_library(Controller
  DataLoader.cpp
  DataWriter.cpp
  DataTypes.cpp
)

target_sources(Controller PUBLIC
  DataLoader.hpp
  DataWriter.hpp
  DataTypes.hpp
)

target_include_directories(Controller INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Controller
    Model
    fmt::fmt
    Eigen3::Eigen
)