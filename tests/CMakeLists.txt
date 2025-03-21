cmake_minimum_required(VERSION 3.20)
project(respondTests CXX)

find_package(GTest REQUIRED CONFIG)

find_package (Eigen3 3.4 REQUIRED)

find_package(spdlog REQUIRED)

add_executable(${PROJECT_NAME}
  src/cost_loader_test.cpp
  src/data_formatter_test.cpp
  src/data_loader_test.cpp
  src/logging_test.cpp
  src/matrices_test.cpp
  src/post_sim_test.cpp
  src/simulation_test.cpp
  src/utility_loader_test.cpp
  src/writer_test.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock_main)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC datamanagement::datamanagement)
target_link_libraries(${PROJECT_NAME} PUBLIC respond)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        "mocks/"
)