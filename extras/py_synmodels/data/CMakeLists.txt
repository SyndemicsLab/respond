cmake_minimum_required(VERSION 3.20)

project(py_data)

pybind11_add_module(${PROJECT_NAME} py_Data.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        py_synmodels
        Eigen3::Eigen
)
install(TARGETS ${PROJECT_NAME} DESTINATION .)