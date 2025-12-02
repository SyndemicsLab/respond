option(RESPOND_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(RESPOND_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# build shared libraries
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" ON)

# testing options
option(RESPOND_BUILD_TESTS "Build tests" OFF)

# coverage options (only valid if RESPOND_BUILD_TESTS is ON)
option(RESPOND_CALCULATE_COVERAGE "Calculate Code Coverage" OFF)

# bench options
option(RESPOND_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# compile level warning and exception options
option(RESPOND_BUILD_WARNINGS "Enable compiler warnings" OFF)
option(RESPOND_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)

# install options
option(RESPOND_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(RESPOND_INSTALL "Generate the install target" ${RESPOND_MASTER_PROJECT})
