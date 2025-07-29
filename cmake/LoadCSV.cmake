message(CHECK_START "Fetching CSV Parser...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    csv
    GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
    GIT_SHALLOW TRUE 
    GIT_TAG 2.3.0
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "CSV Parser Fetched")