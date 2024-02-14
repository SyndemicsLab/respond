#!/usr/bin/bash

cd /home/matt/Repos/RESPONDSimulationv2
rm -rf build/*
conan install . --build=missing --settings=build_type=Debug
cd build
source Debug/generators/conanbuild.sh
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=.. -DCMAKE_TOOLCHAIN_FILE=Debug/generators/conan_toolchain.cmake -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=ON
cmake --build .