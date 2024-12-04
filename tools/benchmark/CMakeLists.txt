cmake_minimum_required(VERSION 3.24)
project(respondBench CXX)

find_package(benchmark REQUIRED)
find_package(spdlog REQUIRED)

add_executable(${PROJECT_NAME}
  src/BENCHMARK_respond.cpp
)

target_include_directories(respond PRIVATE
  ${PROJECT_SOURCE_DIR}/bin/DataManagement/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC matrixify)
target_link_libraries(${PROJECT_NAME} PUBLIC simulation)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)
