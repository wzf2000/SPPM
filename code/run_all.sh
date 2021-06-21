#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
else
    rm -r build
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
bin/Project testcases/scene.txt output/scene.bmp
