project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

add_executable(SimTest SimulationTest.cpp)

target_include_directories(SimTest PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(SimTest gtest gtest_main Model)
gtest_discover_tests(SimTest)