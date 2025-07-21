#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load cmake/3.31.7
    module load gcc/13.2.0
    module load boost/1.83.0
    module load ninja/1.10.2
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
    echo "               Default: Release"
}

# set default build type
BUILDTYPE="Release"

# process optional command line flags
while getopts ":ht:" option; do
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

    case $BUILDTYPE in
        "Debug")
            PRESET="gcc-debug"
        ;;
        "Release")
            PRESET="gcc-release-cluster"
            ;;
    esac
    cmake --workflow --preset "$PRESET" --fresh
)
