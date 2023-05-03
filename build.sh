#!/usr/bin/bash

# ensure the "build/" directory exists
[[ -d "build/" ]] || mkdir "build/"

# use a subshell to enter the build folder and build RESPOND
(
    cd "build/" || return
    rm -rf ./*
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
