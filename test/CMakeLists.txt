project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_executable(respondTest
  SimulationTest.cpp
  DataLoaderTest.cpp
  DataWriterTest.cpp
  ConfigurationTest.cpp
)

target_include_directories(respondTest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(respondTest
  Eigen3::Eigen
  gtest
  gtest_main
  model
  data
)

# create the test data set in the build directory
add_custom_command(
        TARGET respondTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/TestInput
                ${CMAKE_BINARY_DIR}/TestInput)

gtest_discover_tests(respondTest)
