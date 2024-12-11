cmake_minimum_required(VERSION 3.20)

project(kernels)

include(LoadSpdlog)
LoadSpdlog()

find_package(DataManagement 1 REQUIRED
    HINTS "${CMAKE_SOURCE_DIR}/bin/dminstall"
)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()


set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_sources(${PROJECT_NAME}
    PRIVATE
        StateTransitionModel.cpp
        StateTransitionModelImpl.hpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        synmodels
        Eigen3::Eigen 
        spdlog::spdlog 
        DataManagement::DataManagement
        data
)
