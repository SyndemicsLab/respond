cmake_minimum_required(VERSION 3.20)

project(data)

include(LoadSpdlog)
LoadSpdlog()

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/data)
target_sources(${PROJECT_NAME}
    PRIVATE
        EigenFactory.cpp
        RespondDataStore.cpp
        RespondDataStoreImpl.hpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        synmodels
        spdlog::spdlog
        Eigen3::Eigen
)
