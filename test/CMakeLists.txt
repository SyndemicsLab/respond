project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

add_executable(respondTest
  SimulationTest.cpp
  DataLoaderTests.cpp
)
target_include_directories(respondTest PUBLIC
  ${CMAKE_SOURCE_DIR}/src/Model
  ${CMAKE_SOURCE_DIR}/src/Controller
)

# Link Boost
find_package(Boost REQUIRED COMPONENTS
  filesystem)
target_link_libraries(respondTest
  Boost::filesystem
  gtest
  gtest_main
  Model
  Controller
)
gtest_discover_tests(respondTest)
