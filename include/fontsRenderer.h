#pragma once

#include "Constants.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// Function to load a font from a TTF file and create a texture atlas
vector<unsigned char> createAsciiTextureAtlas(const char *fontPath) {
    // Load font file into memory
    FILE *fontFile = fopen(fontPath, "rb");
    if (!fontFile) {
        throw std::runtime_error("Failed to open font file!");
    }

    // Get the font file size and load it into a buffer
    fseek(fontFile, 0, SEEK_END);
    long fontSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    std::vector<unsigned char> fontBuffer(fontSize);
    fread(fontBuffer.data(), 1, fontSize, fontFile);
    fclose(fontFile);

    // Initialize stb_truetype and load the font
    stbtt_fontinfo fontInfo;
    if (!stbtt_InitFont(&fontInfo, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
        throw std::runtime_error("Failed to initialize font!");
    }

    // Create a buffer for the texture atlas (filled with zeros initially)
    std::vector<unsigned char> atlasBuffer(Settings::FONT_ATLAS_WIDTH * Settings::FONT_ATLAS_HEIGHT, 0);

    // Set the font scale based on the desired font size
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, Settings::FONT_FONT_SIZE);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

    int cursorX = 0, cursorY = 0;
    int maxHeightInRow = 0;

    // Loop over ASCII characters from 32 to 127
    for (int c = 32; c < 128; ++c) {
        int width, height, xoff, yoff;

        // Get the bitmap for the character
        unsigned char *bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, scale, c, &width, &height, &xoff, &yoff);

        // If there's no space in the current row, move to the next row
        if (cursorX + width >= Settings::FONT_ATLAS_WIDTH) {
            cursorX = 0;
            cursorY += maxHeightInRow + 1;
            maxHeightInRow = 0;
        }

        // Copy the bitmap into the atlas buffer
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                atlasBuffer[(cursorY + y) * Settings::FONT_ATLAS_WIDTH + (cursorX + x)] = bitmap[y * width + x];
            }
        }

        // Advance the cursor and update row height
        cursorX += width + 1;
        if (height > maxHeightInRow) {
            maxHeightInRow = height;
        }

        // Free the bitmap memory used by stb_truetype
        stbtt_FreeBitmap(bitmap, nullptr);
    }

    return atlasBuffer;
}