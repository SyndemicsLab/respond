cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 11)
include(FetchContent)

# Find OpenMP
find_package(OpenMP REQUIRED)
# Find Eigen
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
# Find Boost
find_package(Boost REQUIRED COMPONENTS log_setup log)
# Find FMT
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_library(Model
    Simulation.cpp 
    Calibration.cpp
)

# Link Eigen
if(WIN32)
target_link_libraries (Model Eigen3)
endif(WIN32)

if(UNIX)
target_link_libraries(Model Eigen3::Eigen)
endif(UNIX)

target_link_libraries(Model OpenMP::OpenMP_CXX)
target_link_libraries(Model ${Boost_LIBRARIES})
target_link_libraries(Model fmt::fmt)

target_include_directories(Model PUBLIC
  ${CMAKE_SOURCE_DIR}/src/Controller
)
