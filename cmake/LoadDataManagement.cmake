message(CHECK_START "Searching for DataManagement...")
include(FetchContent)
FetchContent_Declare(
    datamanagement
    GIT_REPOSITORY https://github.com/SyndemicsLab/DataManagement.git
    GIT_TAG respond_old
)
option(DATAMANAGEMENT_INSTALL "Enable install for datamanagement project" ON)
FetchContent_MakeAvailable(datamanagement)
list(APPEND CMAKE_MODULE_PATH ${datamanagement_SOURCE_DIR}/cmake)
message(CHECK_PASS "Found DataManagement")
