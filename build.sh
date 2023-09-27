#!/usr/bin/bash

SCC_HOSTS=(
    "scc1"
    "scc2"
    "geo"
    "scc4"
)

# ensure the "build/" directory exists
[[ -d "build/" ]] || mkdir "build/"

# use a subshell to enter the build folder and build RESPOND
(
    if [[ "${SCC_HOSTS[*]}" =~ $(hostname) ]]; then
	module load boost/1.79.0_gcc5+
	module load cmake/3.22.2
	module load eigen/3.4.0
	module load gcc/12.2.0
    fi
    cd "build/" || return
    rm -rf ./*
    mkdir outputs
    cmake .. "$@"
    # use another subshell to build the model
    (
	# determine the number of processing units available
	CORES="$(nproc --all)"
	# if CORES > 1 compile in parallel where possible
	([[ ! -z "$CORES" ]] && make -j"$CORES") || make
    )
    # run gtests if built
    [[ -f "test/respondTest" ]] && test/respondTest
)
