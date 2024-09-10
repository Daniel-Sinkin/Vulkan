#!/bin/bash

# This is deprecated in favor of the python script of the same name

# Change to the root directory of the project
cd "$(dirname "$0")/.." || exit

EXTERNAL_INCLUDE_DIR="./external"

# Create the include directory if it doesn't exist
if [[ ! -d "$EXTERNAL_INCLUDE_DIR" ]]; then
    mkdir -p "$EXTERNAL_INCLUDE_DIR"
fi

# Function to calculate the MD5 hash of a file
function calculate_md5 {
    if command -v md5sum &> /dev/null; then
        md5sum "$1" | awk '{ print $1 }'
    elif command -v md5 &> /dev/null; then
        md5 -q "$1"
    else
        echo "Error: No MD5 command found!"
        exit 1
    fi
}

# Expected MD5 hash values for the files
EXPECTED_STB_IMAGE_MD5="27932e6fb3a2f26aee2fc33f2cb4e696"
EXPECTED_TINY_OBJ_LOADER_MD5="d41d8cd98f00b204e9800998ecf8427e"

# Use specific commit hashes to point to particular versions of the headers
STB_IMAGE_COMMIT_HASH="013ac3b"
TINY_OBJ_LOADER_COMMIT_HASH="7b3ba0b"

# URLs for the headers
STB_IMAGE_URL="https://raw.githubusercontent.com/nothings/stb/$STB_IMAGE_COMMIT_HASH/stb_image.h"
TINY_OBJ_LOADER_URL="https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/$TINY_OBJ_LOADER_COMMIT_HASH/tiny_obj_loader.h"

# Local file paths
STB_IMAGE_LOCAL_FILE="$EXTERNAL_INCLUDE_DIR/stb_image.h"
TINY_OBJ_LOADER_LOCAL_FILE="$EXTERNAL_INCLUDE_DIR/tiny_obj_loader.h"

# Function to download and verify a file's MD5 hash
function download_and_verify {
    local url="$1"
    local local_file="$2"
    local expected_md5="$3"
    local file_name=$(basename "$local_file")

    if [[ -f "$local_file" ]]; then
        local local_md5=$(calculate_md5 "$local_file")
        if [[ "$local_md5" == "$expected_md5" ]]; then
            echo "$file_name is already downloaded and the MD5 hash matches."
            return 0
        else
            echo "$file_name is downloaded but the MD5 hash does not match. Downloading again..."
            rm -f "$local_file"
        fi
    fi

    # Download the file
    echo "Downloading $file_name..."
    curl -o "$local_file" "$url"
    if [[ $? -ne 0 ]]; then
        echo "Error downloading $file_name. Exiting."
        exit 1
    fi

    # Verify the MD5 hash of the downloaded file
    local local_md5=$(calculate_md5 "$local_file")
    if [[ "$local_md5" == "$expected_md5" ]]; then
        echo "$file_name downloaded and verified successfully."
    else
        echo "MD5 hash mismatch after downloading $file_name. Exiting."
        exit 1
    fi
}

# Download and verify stb_image.h
download_and_verify "$STB_IMAGE_URL" "$STB_IMAGE_LOCAL_FILE" "$EXPECTED_STB_IMAGE_MD5"

# Download and verify tiny_obj_loader.h
download_and_verify "$TINY_OBJ_LOADER_URL" "$TINY_OBJ_LOADER_LOCAL_FILE" "$EXPECTED_TINY_OBJ_LOADER_MD5"