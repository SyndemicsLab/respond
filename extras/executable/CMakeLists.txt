cmake_minimum_required(VERSION 3.20)
project(respondExecutable LANGUAGES CXX)
add_executable(${PROJECT_NAME} exec.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC respond)