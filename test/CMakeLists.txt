project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_executable(respondTest
  SimulationTest.cpp
  DataLoaderTests.cpp
  DataWriterTest.cpp
)

target_include_directories(respondTest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(respondTest
  Eigen3::Eigen
  gtest
  gtest_main
  Model
  Controller
)

gtest_discover_tests(respondTest)
