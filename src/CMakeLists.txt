cmake_minimum_required(VERSION 3.20)

project(synmodels)

add_subdirectory(data)
add_subdirectory(kernels)
add_subdirectory(models)

add_library(${PROJECT_NAME} STATIC synmodels.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        data
        kernels
        models
        Eigen3::Eigen
        spdlog::spdlog
)
