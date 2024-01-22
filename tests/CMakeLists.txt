cmake_minimum_required(VERSION 3.24)
project(RESPONDTest CXX)

find_package(GTest REQUIRED CONFIG)

# Link Boost
find_package(Boost REQUIRED COMPONENTS filesystem)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

add_executable(respondTest
  src/SimulationTest.cpp
  src/DataLoaderTest.cpp
  src/CostLoaderTest.cpp
  src/UtilityLoaderTest.cpp
  src/DataWriterTest.cpp
  src/ConfigurationTest.cpp
  src/PostSimulationCalculatorTest.cpp
)

target_include_directories(respondTest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  "mocks/"
  "../src/model/include/"
  "../src/data/include/"
)

target_link_libraries(respondTest
  Boost::filesystem
  Eigen3::Eigen
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  model
  data
  spdlog::spdlog
)

