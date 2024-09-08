#!/bin/bash

# Change to the script directory
cd "$(dirname "$0")"

# Compile shaders
./util/compile_shaders.sh

# Clean and prepare the build directory
rm -rf build
mkdir build
cd build

# Determine build type
BUILD_TYPE="Debug"
if [[ "$1" == "--release" ]]; then
    BUILD_TYPE="Release"
fi

# Configure the build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Detect the number of CPU cores (or use 1 if sysctl is not found)
if command -v sysctl &> /dev/null; then
    NUM_CORES=$(sysctl -n hw.ncpu)
    NUM_CORES=$((NUM_CORES - 1))
else
    echo "Warning: 'sysctl' command not found. Defaulting to use 1 core."
    NUM_CORES=1
fi

# Build the project
cmake --build . -j $NUM_CORES