message(CHECK_START "Checking for fmt")
find_package(fmt QUIET)

if(NOT fmt_FOUND)
    include(FetchContent)

    message(CHECK_START "Fetching fmt")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(fmt)

    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "fetched")
else()
    message(CHECK_PASS "fmt found")
endif()