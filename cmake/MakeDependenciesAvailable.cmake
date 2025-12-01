include(FetchContent)

include(${PRIVATE_MODULE_PATH}/LoadEigen.cmake)
include(${PRIVATE_MODULE_PATH}/LoadSpdlog.cmake)

if(RESPOND_BUILD_TESTS STREQUAL "ON")
    include(${PRIVATE_MODULE_PATH}/LoadGtest.cmake)
    FetchContent_MakeAvailable(Eigen3 spdlog googletest)
    include(GoogleTest)
elseif(RESPOND_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(Eigen3 spdlog)
endif()
