function(LoadPybind)
    include(FetchContent)
    message(CHECK_START "Fetching Pybind11")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(pybind11)
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "fetched")
endfunction()