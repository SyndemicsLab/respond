message(CHECK_START "Searching for GoogleTest...")
find_package(GTest)
if(NOT GTest_FOUND)
    include(FetchContent)
    message(CHECK_START "GoogleTest Not Found, Fetching...")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "Fetched GoogleTest")
else()
    message(CHECK_PASS "Found GoogleTest")
endif()
