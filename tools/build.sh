#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the RESPOND Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|-b|-n]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
    echo "b              Build Python Language Bindings"
    echo "n              Build Benchmarking executable"
}

dminstall () {
    if [[ ! -d "DataManagement" ]]; then
        git clone -b respond_old git@github.com:SyndemicsLab/DataManagement
    fi
    echo "DataManagement clone complete."

    # subshell needed to avoid changing working directory unnecessarily
    (
        cd "DataManagement" || return 1
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DDATAMANAGEMENT_BUILD_TESTS=OFF
        cmake --build .
        cmake --install . --prefix "$TOPLEVEL/lib/dminstall"
    )
    rm -rf DataManagement
}

# set default build type
BUILDTYPE="Debug"
RESPOND_BUILD_TESTS="OFF"
BUILD_PYBINDINGS=""
BUILD_BENCHMARK=""

# process optional command line flags
while getopts ":bhnpt:" option; do
    case $option in
        h)
            showhelp
            exit
            ;;
        t)
            case "$OPTARG" in
                "Debug"|"Release")
                    BUILDTYPE="$OPTARG"
                    ;;
                *)
                    echo "Specified build type is invalid!"
                    exit
                    ;;
            esac
            ;;
        p)
            RESPOND_BUILD_TESTS="ON"
            ;;
        b)
            BUILD_PYBINDINGS="ON"
            ;;
        n)
            BUILD_BENCHMARK="ON"
            ;;
        \?)
            echo "Error: Invalid option flag provided!"
            showhelp
            # exit with a non-zero error code to indicate a problem
            exit 1
            ;;
    esac
done

(
    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit
    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"
    ([[ -d "lib/" ]] && rm -rf lib/*.a && rm -rf lib/*.so && rm -rf lib/dminstall)

     # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
        if ! dminstall; then
            echo "Installing \`DataManagement\` failed."
            exit 1
        fi
    fi
    (
        cd "build" || exit
        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_BUILD_TYPE=$BUILDTYPE"
        if [[ -n "$BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DRESPOND_BUILD_TESTS=$BUILD_TESTS"
        fi
        # build Python language bindings, if specified
        if [[ -n "$BUILD_PYBINDINGS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_PYBINDINGS=$BUILD_PYBINDINGS -DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        fi
        # build benchmarking executable
        if [[ -n "$BUILD_BENCHMARK" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_BENCHMARK=$BUILD_BENCHMARK"
        fi
        # run the full build command as specified
        $CMAKE_COMMAND
        (
            # determine the number of processing units available
            CORES="$(nproc --all)"
            # if CORES > 1 compile in parallel where possible
            ([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
        )
        # run tests, if they built properly
    )
    if [[ (-n "$RESPOND_BUILD_TESTS") && (-f "build/tests/respondTests") ]]; then
        build/tests/respondTests
    fi
)
