
message(CHECK_START "Fetching Eigen...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG "3.4.0"
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
    # FIND_PACKAGE_ARGS NAMES Eigen3
)

# Turn off Eigen Testing and Docs
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Eigen Fetched")