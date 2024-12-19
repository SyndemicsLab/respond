message(CHECK_START "Searching for Eigen3...")
find_package(Eigen3)

if( NOT Eigen3_FOUND )
    include(FetchContent)
    message(CHECK_START "Eigen3 Not Found, Fetching...")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
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
    FetchContent_MakeAvailable(Eigen)
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "Eigen 3 Fetched")
else()
    message(CHECK_PASS "Eigen3 Found")
endif()