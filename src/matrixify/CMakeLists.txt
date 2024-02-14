cmake_minimum_required(VERSION 3.20)

project(matrixify)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
  HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(${PROJECT_NAME}
  Loader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  DataWriter.cpp
  DataFormatter.cpp
  DataTypes.cpp
  Matrix3dFactory.cpp
  Matrix3dPrinter.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
