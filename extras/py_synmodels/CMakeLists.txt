cmake_minimum_required(VERSION 3.20)

project(py_synmodels)

add_subdirectory(data)
add_subdirectory(kernels)
add_subdirectory(models)

pybind11_add_module(${PROJECT_NAME} py_SynModels.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        synmodels
        Eigen3::Eigen
)
install(TARGETS ${PROJECT_NAME} DESTINATION .)