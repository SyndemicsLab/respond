cmake_minimum_required(VERSION 3.27)
project(respond_exe LANGUAGES CXX)
add_executable(${PROJECT_NAME} exec.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC respond)