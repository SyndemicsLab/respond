function(LoadOptions)
    option(SYNMODELS_BUILD_ALL "Build all artifacts" OFF)

    # build position independent code
    option(SYNMODELS_BUILD_PIC "Build position independent code (-fPIC)" OFF)

    # example options
    option(SYNMODELS_BUILD_EXAMPLE "Build example" ${SYNMODELS_MASTER_PROJECT})

    # testing options
    option(SYNMODELS_BUILD_TESTS "Build tests" OFF)

    # bench options
    option(SYNMODELS_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

    # warning options
    option(SYNMODELS_BUILD_WARNINGS "Enable compiler warnings" OFF)

    # install options
    option(SYNMODELS_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
    option(SYNMODELS_INSTALL "Generate the install target" ${SYNMODELS_MASTER_PROJECT})
    option(SYNMODELS_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any synmodels exceptions" OFF)
endfunction()