include(FetchContent)

include(LoadEigen)
include(LoadSpdlog)
include(LoadDataManagement)
include(LoadCSV)

if(RESPOND_BUILD_TESTS STREQUAL "ON")
    include(LoadGtest)
    FetchContent_MakeAvailable(Eigen3 spdlog datamanagement csv googletest)
    include(GoogleTest)
elseif(RESPOND_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(Eigen3 spdlog datamanagement csv)
endif()
