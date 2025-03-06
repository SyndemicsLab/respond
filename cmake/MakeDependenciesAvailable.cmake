set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(LoadEigen)
include(LoadSpdlog)

FetchContent_MakeAvailable(Eigen spdlog)