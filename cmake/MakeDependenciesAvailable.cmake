include(FetchContent)

include(LoadEigen)
include(LoadSpdlog)

if(RESPOND_BUILD_TESTS STREQUAL "ON")
    include(LoadGtest)
    FetchContent_MakeAvailable(Eigen3 spdlog googletest)
    include(GoogleTest)
elseif(RESPOND_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(Eigen3 spdlog)
endif()
