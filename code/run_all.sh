#!/usr/bin/env bash
ROUNDS=2500
PHOTONS=2000000
CKPT_INTERVAL=1

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

if [[ ! -d checkpoints ]]; then
    mkdir -p checkpoints
fi

# Build project.
cd build
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
bin/Project testcases/$1 output/scene.bmp $ROUNDS $PHOTONS $CKPT_INTERVAL
