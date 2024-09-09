#!/bin/bash

# Change to the script directory
cd "$(dirname "$0")"

# Call the download dependencies script
./scripts/download_dependencies.sh
if [[ $? -ne 0 ]]; then
    echo "Error downloading dependencies. Exiting."
    exit 1
fi

# Call the download assets script (for textures)
./scripts/download_assets.sh
if [[ $? -ne 0 ]]; then
    echo "Error downloading assets. Exiting."
    exit 1
fi

# Compile shaders
./scripts/compile_shaders.sh

# Clean and prepare the build directory
rm -r build
mkdir build
cd build

# Determine build type
BUILD_TYPE="Debug"
if [[ "$1" == "--release" ]]; then
    BUILD_TYPE="Release"
fi

# Detect the number of CPU cores (or use 1 if sysctl is not found)
if command -v sysctl &> /dev/null; then
    NUM_CORES=$(sysctl -n hw.ncpu)
    NUM_CORES=$((NUM_CORES - 1))
else
    echo "Warning: 'sysctl' command not found. Defaulting to use 1 core."
    NUM_CORES=1
fi

cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
cmake --build . -j $NUM_CORES