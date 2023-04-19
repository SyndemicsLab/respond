#!/usr/bin/bash

# ensure the "build/" directory exists
[[ -d "build/" ]] || mkdir "build/"
# use a subshell to enter the build folder and build RESPOND
(
    cd "build/" || return
    rm -rf ./*
    cmake .. $@
    make
    # run gtests if built
    [[ -f "test/respondTest" ]] && test/respondTest
)
