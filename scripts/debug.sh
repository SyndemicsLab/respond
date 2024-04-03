#!/usr/bin/bash

if ([ ! -d build ]); then
    cd ..
fi
if ([ -f lib/Matrixify.cpython-311-x86_64-linux-gnu.so ]); then
    rm -rf lib/Matrixify.cpython-311-x86_64-linux-gnu.so
fi
if ([ -f lib/Simulation.cpython-311-x86_64-linux-gnu.so ]); then
    rm -rf lib/Simulation.cpython-311-x86_64-linux-gnu.so
fi

([[ -d build ]] && rm -rf build/*) || mkdir build
rm -rf build/*
rm -rf bin/*
conan install . --build=missing --settings=build_type=Debug
cd build
pwd
source Debug/generators/conanbuild.sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_TESTS=ON -DBUILD_PYBINDINGS=ON
cmake --build . 
source Debug/generators/deactivate_conanbuild.sh
./../bin/respondTest
