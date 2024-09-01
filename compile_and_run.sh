#!/bin/bash

# Run the build script
echo "Running build.sh..."
./build.sh

# Check if the build was successful
if [ $? -eq 0 ]; then
    echo "Build succeeded. Running the project..."
    # Run the executable
    ./build/OpenGLProject
else
    echo "Build failed. Please check the errors."
fi