#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load openmpi/4.1.5
    module load cmake/3.22.2
fi

err () {
    echo -e "$RED$1$NC"
}

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the RESPOND Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
    echo "b              Build Python Bindings."
}

# set default build type
CMAKE_BUILD_TYPE="Debug"
SYNMODELS_BUILD_TESTS=""
BUILD_SHARED_LIBS="OFF"
SYNMODELS_BUILD_PYBINDINGS="OFF"
SYNMODELS_BUILD_PIC="OFF"
# BUILD_BENCHMARK=""

# process optional command line flags
while getopts ":hpbt:" option; do
    case $option in
        h)
            showhelp
            exit
            ;;
        t)
            case "$OPTARG" in
                "Debug"|"Release")
                    CMAKE_BUILD_TYPE="$OPTARG"
                    ;;
                *)
                    echo "Specified build type is invalid!"
                    exit
                    ;;
            esac
            ;;
        p)
            SYNMODELS_BUILD_TESTS="ON"
            ;;
        b)
            SYNMODELS_BUILD_PYBINDINGS="ON"
            SYNMODELS_BUILD_PIC="ON"
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

    (
        cd "build" || exit
        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_BUILD_TYPE=$BUILDTYPE -DSYNMODELS_BUILD_PYBINDINGS=$SYNMODELS_BUILD_PYBINDINGS -DSYNMODELS_BUILD_PIC=$SYNMODELS_BUILD_PIC"
        if [[ -n "$SYNMODELS_BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DSYNMODELS_BUILD_TESTS=$SYNMODELS_BUILD_TESTS"
        fi

        # build static or shared library
        CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"

        err "[EXECUTE] $CMAKE_COMMAND"

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
    if [[ (-n "$SYNMODELS_BUILD_TESTS") && (-f "build/tests/respondTest") ]]; then
        build/tests/respondTest
    fi
)
