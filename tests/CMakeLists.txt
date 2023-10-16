# project("GoogleTest")
# enable_testing()
# include(GoogleTest)

cmake_minimum_required(VERSION 3.24)
project(RESPONDTest CXX)

find_package(GTest REQUIRED CONFIG)

# Link Boost
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(mocks)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

add_executable(respondTest
  src/SimulationTest.cpp
  src/DataLoaderTest.cpp
  src/CostLoaderTest.cpp
  src/UtilityLoaderTest.cpp
  src/DataWriterTest.cpp
  src/ConfigurationTest.cpp
  src/PostSimulationCalculatorTest.cpp
)

target_include_directories(respondTest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  "mocks/"
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
)

# create the test data set in the build directory
#add_custom_command(
#        TARGET respondTest POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#                ${CMAKE_SOURCE_DIR}/tests/TestInput
#                ${CMAKE_BINARY_DIR}/TestInput)

# gtest_discover_tests(respondTest)
