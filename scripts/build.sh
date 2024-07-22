#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load miniconda
    conda env create -f "/projectnb/respond/environment.yml" 2&>/dev/null
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the RESPOND Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|-b|-n]"
    echo "a              Build API executable"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
    echo "b              Build Python Language Bindings"
    echo "n              Build Benchmarking executable"
}

# set default build type
BUILDTYPE="Debug"
BUILD_TESTS=""
BUILD_API=""
BUILD_PYBINDINGS=""
BUILD_BENCHMARK=""

# process optional command line flags
while getopts ":abhnpt:" option; do
    case $option in
        a)
            BUILD_API="ON"
            ;;
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
            BUILD_TESTS="ON"
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
    echo "Checking if \`conda\` is found..."
    # ensure conda is present on the system
    if ! command -v conda &>/dev/null; then
        echo "\`conda\` not present on the system! Exiting..."
        exit 1
    else
	echo "\`conda\` found!"
    fi
    # activate the conda environment
    conda activate respond 2&>/dev/null

    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

    # detect or install DataManagement
    echo "Checking for the presence of \`DataManagement\`..."
    if [[ ! -d "lib/DataManagement" ]]; then
	echo "\`DataManagement\` not found. Attempting to configure..."
        # check if submodules have been initialized
        if git submodule status | grep --quiet '^-'; then
            git submodule init
        fi
        # ensure submodules use the specified hash
        git submodule update --recursive
        # the commented stub below installs shared objects of DataManagement,
        # instead of a static library
        # if ! (
        #         cd "DataManagement" || exit 1
        #         ./install.sh -i "$TOPLEVEL/lib/dminstall"
        #     ); then
        #     echo "Installing \`DataManagement\` failed."
        # fi
        # rm -rf DataManagement
    fi
    echo "\`DataManagement\` found!"

    (
        cd "build" || exit
        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_BUILD_TYPE=$BUILDTYPE"
        if [[ -n "$BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_TESTS=$BUILD_TESTS"
        fi
        # build Python language bindings, if specified
        if [[ -n "$BUILD_PYBINDINGS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_PYBINDINGS=$BUILD_PYBINDINGS -DCMAKE_POSITION_INDEPENDENT_CODE=ON"
        fi
        # build benchmarking executable
        if [[ -n "$BUILD_BENCHMARK" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_BENCHMARK=$BUILD_BENCHMARK"
        fi
        # build API executable
        if [[ -n "$BUILD_API" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_API=$BUILD_API"
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
    if [[ (-n "$BUILD_TESTS") && (-f "bin/respondTest") ]]; then
        bin/respondTest
    fi
)
