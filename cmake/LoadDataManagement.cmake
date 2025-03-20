message(CHECK_START "Searching for DataManagement...")
find_package(datamanagement 0.1.0 REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/dminstall")
message(CHECK_PASS "Found DataManagement")
