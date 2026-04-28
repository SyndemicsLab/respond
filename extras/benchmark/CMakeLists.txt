cmake_minimum_required(VERSION 3.27)
project(respond_benchmark LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    src/benchmark_respond.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        respond::respond_model
)
