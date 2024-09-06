#!/bin/bash
cd "$(dirname "$0")"

# Clean and create build directory
rm -rf build
mkdir build
cd build

# Check if the --release argument is passed
BUILD_TYPE="Debug"
if [[ "$1" == "--release" ]]; then
    BUILD_TYPE="Release"
fi

# Configure the project with the appropriate build type
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Determine the number of CPU cores, fallback to 1 if sysctl is not available
if command -v sysctl &> /dev/null; then
    NUM_CORES=$(sysctl -n hw.ncpu)
    NUM_CORES=$((NUM_CORES - 1))
else
    echo "Warning: 'sysctl' command not found. Defaulting to use 1 core."
    NUM_CORES=1
fi

# Build the project using the determined number of cores
cmake --build . -j $NUM_CORES

# Run the compiled binary
./VulkanEngine