#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
if ! command_exists cmake; then
    echo "Error: CMake is not installed. Please install CMake and try again."
    exit 1
fi

if ! command_exists make; then
    echo "Error: Make is not installed. Please install Make and try again."
    exit 1
fi

if ! command_exists glslc; then
    echo "Error: glslc (Vulkan shader compiler) is not installed. Please install the Vulkan SDK and try again."
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build || { echo "Error: Failed to change to build directory"; exit 1; }

# Run CMake to generate build files
cmake .. || { echo "Error: CMake configuration failed"; exit 1; }

# Build the project
make || { echo "Error: Build failed"; exit 1; }

# Return to the root directory
cd .. || { echo "Error: Failed to return to root directory"; exit 1; }

# Compile shaders
echo "Compiling shaders..."
mkdir -p shaders/compiled

# Find all .vert and .frag files in the shaders directory
shaders=$(find shaders -name "*.vert" -o -name "*.frag")

for shader in $shaders
do
    filename=$(basename -- "$shader")
    name="${filename%.*}"
    extension="${filename##*.}"
    output="shaders/compiled/${name}.spv"
    
    glslc "$shader" -o "$output" || { echo "Error: Failed to compile shader $filename"; exit 1; }
    echo "Compiled: $filename -> ${name}.spv"
done

echo "All shaders compiled successfully!"

echo "Build completed successfully!"

# Run the executable
./build/VulkanEngine || { echo "Error: Failed to run the executable"; exit 1; }