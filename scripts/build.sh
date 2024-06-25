#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load miniconda
    conda env create -f "/projectnb/respond/environment.yml" 2&>/dev/null
    conda activate respond
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

# set default build type
BUILDTYPE="Debug"
BUILD_TESTS=""
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
            exit
            ;;
    esac
done

(
    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit
    CONANPATH=$(command -v conan)

    # install conan, if not found in the current scope
    if [[ -z "$CONANPATH" ]]; then
        echo "The \`conan\` command is not found\! Attempting a local installation..."
        if [[ -z "$VIRTUAL_ENV" ]]; then
            if ! pip install --user conan; then
                echo "\`conan\` installation failed. Exiting."
                exit 1
            fi
        else
            if ! pip install conan; then
                echo "\`conan\` installation failed. Exiting."
                exit 1
            fi
        fi
        CONANPATH="python3 -m conans.conan"
    fi

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

    # install dependencies via conan
    if [[ ! -f "$HOME/.conan2/profiles/default" ]]; then
        $CONANPATH profile detect --force
    fi

    echo "Installing missing dependencies via \`conan\`..."
    INSTALL_DEPS="$CONANPATH install . --build=missing --settings=build_type=$BUILDTYPE"
    if [[ -n "$BUILD_BENCHMARK" ]]; then
        INSTALL_DEPS="$INSTALL_DEPS -o benchmark=True"
    fi
    if ! $INSTALL_DEPS; then
        echo "Issue installing dependencies! Exiting..."
        exit 1
    fi
    echo "Dependencies installed!"

    # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
        git clone git@github.com:SyndemicsLab/DataManagement
        if ! (
                cd "DataManagement" || exit 1
                ./install.sh -i "$TOPLEVEL/lib/dminstall"
            ); then
            echo "Installing \`DataManagement\` failed."
        fi
        rm -rf DataManagement
    fi

    # detect or install DataManagement
    if [[ ! -d "lib/dminstall" ]]; then
	git clone git@github.com:SyndemicsLab/DataManagement
	if ! (
		cd "DataManagement" || exit 1
		./install.sh "$TOPLEVEL/lib/dminstall"
	    ); then
	    echo "Installing \`DataManagement\` failed."
	fi
	rm -rf DataManagement
    fi

    (
        cd "build" || exit
        # check if the conan generator file was generated successfully
        if [[ -f "$BUILDTYPE/generators/conanbuild.sh" ]]; then
            # shellcheck source=/dev/null
            source "$BUILDTYPE/generators/conanbuild.sh"
        else
            echo "\`conan\` generator failed. Terminating."
            exit 1
        fi

        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_TOOLCHAIN_FILE=$BUILDTYPE/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILDTYPE"
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
        # run the full build command as specified
        $CMAKE_COMMAND
        (
            # determine the number of processing units available
            CORES="$(nproc --all)"
            # if CORES > 1 compile in parallel where possible
            ([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
        )
        # deactivate the conan virtual environment
        # shellcheck source=/dev/null
        source "$BUILDTYPE/generators/deactivate_conanbuild.sh"
        # run tests, if they built properly
    )
    if [[ (-n "$BUILD_TESTS") && (-f "bin/respondTest") ]]; then
        bin/respondTest
    fi
)
