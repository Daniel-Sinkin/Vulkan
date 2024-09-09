#!/bin/bash

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

EXPECTED_FILE_MD5="27932e6fb3a2f26aee2fc33f2cb4e696"

# Use a specific commit hash to point to a particular version of stb_image.h
STB_IMAGE_COMMIT_HASH="013ac3b"
STB_IMAGE_URL="https://raw.githubusercontent.com/nothings/stb/$STB_IMAGE_COMMIT_HASH/stb_image.h"
STB_IMAGE_LOCAL_FILE="$EXTERNAL_INCLUDE_DIR/stb_image.h"

# Check if stb_image.h exists and its MD5 hash matches the expected value
if [[ -f "$STB_IMAGE_LOCAL_FILE" ]]; then
    LOCAL_FILE_MD5=$(calculate_md5 "$STB_IMAGE_LOCAL_FILE")
    if [[ "$LOCAL_FILE_MD5" == "$EXPECTED_FILE_MD5" ]]; then
        echo "stb_image.h is already downloaded and the MD5 hash matches."
        exit 0
    else
        echo "stb_image.h is downloaded but the MD5 hash does not match. Downloading again..."
        rm -f "$STB_IMAGE_LOCAL_FILE"
    fi
fi

# Download stb_image.h if it doesn't exist or the MD5 hash doesn't match
if [[ ! -f "$STB_IMAGE_LOCAL_FILE" ]]; then
    echo "Downloading stb_image.h..."
    curl -o "$STB_IMAGE_LOCAL_FILE" "$STB_IMAGE_URL"
    if [[ $? -ne 0 ]]; then
        echo "Error downloading stb_image.h. Exiting."
        exit 1
    fi

    # Verify the MD5 hash of the downloaded file
    LOCAL_FILE_MD5=$(calculate_md5 "$STB_IMAGE_LOCAL_FILE")
    if [[ "$LOCAL_FILE_MD5" == "$EXPECTED_FILE_MD5" ]]; then
        echo "stb_image.h downloaded and verified successfully."
    else
        echo "MD5 hash mismatch after downloading stb_image.h. Exiting."
        exit 1
    fi
fi