include(FetchContent)

include(LoadEigen)
include(LoadSpdlog)
include(LoadDataManagement)

FetchContent_MakeAvailable(eigen spdlog datamanagement)
set(EIGEN3_INCLUDE_DIR ${eigen_SOURCE_DIR})