cmake_minimum_required(VERSION 3.27)
project(respond_tests VERSION ${RESPOND_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT RESPOND_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but RESPOND_BUILD_TESTS is OFF!")
    return()
endif()

add_executable(respond_tests)

target_sources(respond_tests
    PRIVATE
        logging_test.cpp
        cost_effectiveness_test.cpp
        markov_test.cpp
        respond_test.cpp
        mocks/markov_mock.hpp
    )

target_link_libraries(respond_tests 
PUBLIC 
    respond_model
    Eigen3::Eigen
    spdlog::spdlog
    GTest::gmock_main
)

gtest_discover_tests(${PROJECT_NAME})