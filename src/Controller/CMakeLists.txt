cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)

# Link Boost
find_package(Boost REQUIRED COMPONENTS filesystem)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find FMT
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

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
    Boost::filesystem
    Model
    fmt::fmt
    Eigen3::Eigen
)