cmake_minimum_required(VERSION 3.20)

project(models)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/models)
target_sources(${PROJECT_NAME}
    PRIVATE
        Respond.cpp
        RespondImpl.hpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        synmodels
        Eigen3::Eigen
        data
        kernels
)

