
message(CHECK_START "Fetching Eigen3...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    FIND_PACKAGE_ARGS NAMES Eigen3
)

# Turn off Eigen Testing and Docs
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_MPL2_ONLY ON)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Eigen3 Fetched")