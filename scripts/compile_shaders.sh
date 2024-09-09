#!/bin/bash

# Change to the project root directory (two levels up from 'util')
cd "$(dirname "$0")/.."

# Fallback if $GLSLC is not set
GLSLC=${GLSLC:-"$VULKAN_SDK/bin/glslc"}

# Define the shaders directory relative to the project root
SHADERS_DIR="shaders"
COMPILED_DIR="$SHADERS_DIR/compiled"

# Create the compiled directory if it doesn't exist
mkdir -p "$COMPILED_DIR"

# Delete all files in the compiled directory before recompiling
echo "Deleting all previously compiled shaders..."
rm -f "$COMPILED_DIR"/*

# Get the list of shaders to compile (frag, vert, comp, geom)
shader_files=($(find "$SHADERS_DIR" -type f \( -name "*.frag" -o -name "*.vert" -o -name "*.comp" -o -name "*.geom" \)))

# Get the total number of shaders
total_shaders=${#shader_files[@]}
current_shader=1

# Compile the shaders
echo "Compiling shaders:"

# Iterate over all shader files
for shader_file in "${shader_files[@]}"; do
    # Get the file name without the path
    file_name=$(basename "$shader_file")
    
    # Define the output file path
    output_file="$COMPILED_DIR/$file_name.spv"
    
    # Display the compilation progress
    echo "    ($current_shader / $total_shaders): $file_name -> $output_file"
    
    # Compile the shader
    "$GLSLC" "$shader_file" -o "$output_file"
    
    # Increment the shader counter
    current_shader=$((current_shader + 1))
done

echo "Shader compilation complete!"