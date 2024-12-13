cmake_minimum_required(VERSION 3.20)

project(py_synmodels)

pybind11_add_module(${PROJECT_NAME} 
    py_SynModels.cpp
    data/py_Data.hpp
    kernels/py_Kernels.hpp
    models/py_Models.hpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        synmodels
        Eigen3::Eigen
)
install(TARGETS ${PROJECT_NAME} DESTINATION .)