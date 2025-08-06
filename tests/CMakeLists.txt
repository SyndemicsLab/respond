cmake_minimum_required(VERSION 3.27)
project(respond_tests VERSION ${RESPOND_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT RESPOND_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but RESPOND_BUILD_TESTS is OFF!")
    return()
endif()

add_executable(${PROJECT_NAME}
  src/logging_test.cpp
  src/cost_effectiveness_test.cpp
  src/markov_test.cpp
  src/respond_test.cpp
)

target_link_libraries(${PROJECT_NAME} 
PUBLIC 
    respond
    Eigen3::Eigen
    spdlog::spdlog
    GTest::gmock_main
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        "mocks/"
)

gtest_discover_tests(${PROJECT_NAME})