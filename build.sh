#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Remove the old build directory if it exists
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

# Create a new build directory
mkdir build
cd build

# Run CMake to configure the project
echo "Configuring the project with CMake..."
cmake ..

# Build the project using Make
echo "Building the project..."
make

# Navigate back to the root directory
cd ..

# Run the compiled executable
echo "Build complete. To run the project, execute:"
echo "./build/OpenGLProject"
