include(FetchContent)

include(LoadEigen)
include(LoadSpdlog)
include(LoadDataManagement)

FetchContent_MakeAvailable(Eigen3 spdlog datamanagement)