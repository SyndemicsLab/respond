include(GoogleTest)
add_executable(SimTest SimulationTest.cpp)
gtest_discover_tests(SimTest)