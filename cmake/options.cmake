option(RESPOND_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(RESPOND_BUILD_PIC "Build position independent code (-fPIC)" OFF)

option(RESPOND_BUILD_EXECUTABLE "Build Executable for RESPOND" ON)

# testing options
option(RESPOND_BUILD_TESTS "Build tests" ON)

# bench options
option(RESPOND_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# warning options
option(RESPOND_BUILD_WARNINGS "Enable compiler warnings" OFF)

# install options
option(RESPOND_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(RESPOND_INSTALL "Generate the install target" ${RESPOND_MASTER_PROJECT})
option(RESPOND_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)
