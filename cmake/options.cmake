option(RESPONDSIMULATION_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(RESPONDSIMULATION_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# testing options
option(RESPONDSIMULATION_BUILD_TESTS "Build tests" ON)

# bench options
option(RESPONDSIMULATION_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# warning options
option(RESPONDSIMULATION_BUILD_WARNINGS "Enable compiler warnings" OFF)

# install options
option(RESPONDSIMULATION_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(RESPONDSIMULATION_INSTALL "Generate the install target" ${RESPONDSIMULATION_MASTER_PROJECT})
option(RESPONDSIMULATION_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)
