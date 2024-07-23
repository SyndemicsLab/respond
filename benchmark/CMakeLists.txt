cmake_minimum_required(VERSION 3.24)
project(respondBench CXX)

find_package(benchmark REQUIRED)
find_package(spdlog REQUIRED)
find_package(DataManagement 1 REQUIRED
  HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_executable(${PROJECT_NAME}
  src/BENCHMARK_respond.cpp
)

target_include_directories(respond
  ${PROJECT_SOURCE_DIR}/lib/DataManagement/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC matrixify)
target_link_libraries(${PROJECT_NAME} PUBLIC simrunner)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)
