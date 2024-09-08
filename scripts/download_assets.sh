#!/bin/bash

# Define the destination folder and URL
TEXTURE_FOLDER="textures"
URL="https://vulkan-tutorial.com/images/texture.jpg"
FILE_NAME="texture.jpg"

# Create the textures folder if it doesn't exist
if [ ! -d "$TEXTURE_FOLDER" ]; then
    echo "Creating textures directory..."
    mkdir -p "$TEXTURE_FOLDER"
fi

# Download the texture and save it in the textures folder
echo "Downloading texture from $URL..."
curl -o "$TEXTURE_FOLDER/$FILE_NAME" "$URL"

# Check if the download was successful
if [ $? -eq 0 ]; then
    echo "Download successful. Texture saved in $TEXTURE_FOLDER/$FILE_NAME"
else
    echo "Download failed. Please check the URL and try again."
fi