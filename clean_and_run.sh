#!/bin/bash
cd "$(dirname "$0")"

rm -rf build
mkdir build
cd build

BUILD_TYPE="Debug"
if [[ "$1" == "--release" ]]; then
    BUILD_TYPE="Release"
fi

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

if command -v sysctl &> /dev/null; then
    NUM_CORES=$(sysctl -n hw.ncpu)
    NUM_CORES=$((NUM_CORES - 1))
else
    echo "Warning: 'sysctl' command not found. Defaulting to use 1 core."
    NUM_CORES=1
fi

cmake --build . -j $NUM_CORES

./VulkanEngine