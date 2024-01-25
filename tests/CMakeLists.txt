cmake_minimum_required(VERSION 3.24)
project(respondTest CXX)

find_package(GTest REQUIRED CONFIG)

# Link Boost
find_package(Boost REQUIRED COMPONENTS filesystem)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
  HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_executable(${PROJECT_NAME}
  src/SimulationTest.cpp
  src/DataLoaderTest.cpp
  src/CostLoaderTest.cpp
  src/UtilityLoaderTest.cpp
  src/DataWriterTest.cpp
  src/ConfigurationTest.cpp
  src/PostSimulationCalculatorTest.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)
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