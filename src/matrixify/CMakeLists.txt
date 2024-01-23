cmake_minimum_required(VERSION 3.20)

project(matrixify)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(matrixify
  Loader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  DataWriter.cpp
  DataFormatter.cpp
  DataTypes.cpp
  Configuration.cpp
  Matrix3dFactory.cpp
  Matrix3dPrinter.cpp
)

target_include_directories(matrixify PRIVATE
  ${Boost_INCLUDE_DIRS}
  ${Eigen3_INCLUDE_DIRS}
  "include/"
)
target_link_libraries(matrixify
  Eigen3::Eigen
  spdlog::spdlog
)
