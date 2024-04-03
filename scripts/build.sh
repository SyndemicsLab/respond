#!/usr/bin/bash
# only execute these lines if the `module` command is present in the environment
if command -v module &>/dev/null; then
    module load python3/3.10.12
    module load gcc/12.2.0
fi

if [[ -z "$1" ]]; then
    BUILDTYPE="Debug"
else
    case "$1" in
	"Debug"|"Release"|"Build")
	    BUILDTYPE="$1"
	    ;;
	*)
	    echo "Specified build type is invalid\!"
	    ;;
    esac
fi

# change to the top-level git folder
cd "$(git rev-parse --show-toplevel)"
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
$CONANPATH profile detect --force
$CONANPATH install . --build=missing --settings=build_type="$BUILDTYPE"

(
    cd "build" || exit
    # check if the conan generator file was generated successfully
    if [[ -f "$BUILDTYPE/generators/conanbuild.sh" ]]; then
	source "$BUILDTYPE/generators/conanbuild.sh"
    else
	echo "\`conan\` generator failed. Terminating."
	exit 1
    fi
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$BUILDTYPE"/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE="$BUILDTYPE"
    (
	# determine the number of processing units available
	CORES="$(nproc --all)"
	# if CORES > 1 compile in parallel where possible
	([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
    )
    # deactivate the conan virtual environment
    source "$BUILDTYPE/generators/deactivate_conanbuild.sh"
)
