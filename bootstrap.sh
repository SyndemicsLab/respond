#!/usr/bin/bash

[[ -d "build/" ]] || mkdir "build/"
(
    cd "build/" || return
    rm -rf ./*
    cmake ..
    make
    test/respondTest
)
