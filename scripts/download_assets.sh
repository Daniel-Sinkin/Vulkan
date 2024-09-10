#!/bin/bash

# This is deprecated in favor of the python script named "download_dependencies.py"

# Define the destination folder, URL, file name, and expected MD5 hash
TEXTURE_FOLDER="textures"
URL="https://vulkan-tutorial.com/images/texture.jpg"
FILE_NAME="texture.jpg"
EXPECTED_MD5="4e60ee5adc9dd77c029faeb96fd113c0"

# Create the textures folder if it doesn't exist
if [ ! -d "$TEXTURE_FOLDER" ]; then
    echo "Creating textures directory..."
    mkdir -p "$TEXTURE_FOLDER"
fi

# Check if the file already exists
if [ -f "$TEXTURE_FOLDER/$FILE_NAME" ]; then
    echo "File exists. Computing MD5 hash..."
    FILE_MD5=$(md5 -q "$TEXTURE_FOLDER/$FILE_NAME")

    # Compare the computed MD5 with the expected MD5
    if [ "$FILE_MD5" == "$EXPECTED_MD5" ]; then
        echo "MD5 hash matches. The file is already up to date."
    else
        echo "MD5 hash does not match. Deleting the old file and downloading again..."
        rm "$TEXTURE_FOLDER/$FILE_NAME"
        echo "Downloading texture from $URL..."
        curl -o "$TEXTURE_FOLDER/$FILE_NAME" "$URL"
        if [ $? -eq 0 ]; then
            echo "Download successful. Texture saved in $TEXTURE_FOLDER/$FILE_NAME"
        else
            echo "Download failed. Please check the URL and try again."
        fi
    fi
else
    # File does not exist, download it
    echo "File does not exist. Downloading texture from $URL..."
    curl -o "$TEXTURE_FOLDER/$FILE_NAME" "$URL"
    if [ $? -eq 0 ]; then
        echo "Download successful. Texture saved in $TEXTURE_FOLDER/$FILE_NAME"
    else
        echo "Download failed. Please check the URL and try again."
    fi
fi