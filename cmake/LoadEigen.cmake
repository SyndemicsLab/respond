
message(CHECK_START "Fetching Eigen3...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    FIND_PACKAGE_ARGS NAMES Eigen3
)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Eigen3 Fetched")