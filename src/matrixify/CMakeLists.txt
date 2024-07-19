cmake_minimum_required(VERSION 3.20)

project(matrixify)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME}
  BaseLoader.cpp
  CostLoader.cpp
  UtilityLoader.cpp
  DataLoader.cpp
  Writer.cpp
  DataFormatter.cpp
  DataTypes.cpp
  Matrix3dFactory.cpp
  Matrix3dPrinter.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "include/"
        ${PROJECT_SOURCE_DIR}/lib/DataManagement/include/
    )
