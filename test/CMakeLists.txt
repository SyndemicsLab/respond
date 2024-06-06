cmake_minimum_required(VERSION 3.24)
project(respondTest CXX)

find_package(GTest REQUIRED CONFIG)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
  HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_executable(${PROJECT_NAME}
  src/TEST_Simulation.cpp
  src/TEST_DataLoader.cpp
  src/TEST_CostLoader.cpp
  src/TEST_UtilityLoader.cpp
  src/TEST_DataWriter.cpp
  src/TEST_PostSimulationCalculator.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock_main)
target_link_libraries(${PROJECT_NAME} PUBLIC simrunner)
target_link_libraries(${PROJECT_NAME} PUBLIC matrixify)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

target_include_directories(${PROJECT_NAME} PUBLIC "mocks/")