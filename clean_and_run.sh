#!/bin/bash
cd "$(dirname "$0")"

# Fallback if $GLSLC is not set
GLSLC=${GLSLC:-"$VULKAN_SDK/bin/glslc"}

# Compile the shaders
echo "Compiling the shaders"
"$GLSLC" shaders/shader.frag -o shaders/compiled/frag.spv
"$GLSLC" shaders/shader.vert -o shaders/compiled/vert.spv
echo ""

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

# Run the VulkanEngine executable
./VulkanEngine