message(CHECK_START "Searching for spdlog...")
find_package(spdlog)
if( NOT spdlog_FOUND )
    include(FetchContent)
    message(CHECK_START "spdlog Not Found, Fetching...")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY  https://github.com/gabime/spdlog.git
            GIT_TAG         v1.x
            GIT_PROGRESS    TRUE
    )
    FetchContent_MakeAvailable(spdlog)
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "spdlog Fetched")
else()
    message(CHECK_PASS "spdlog Found")
endif()