"""
Used to verify the binary blob that gets spit out by the `pushFramebufferToCpu` function is correct.
"""

import numpy as np
from PIL import Image

# Set the correct dimensions based on the pixel data size
width = 800
height = 2400  # Adjusted based on the total size of the pixel data

# Load the binary file
pixel_data = np.fromfile("framebuffer.bin", dtype=np.uint8)

# Reshape to match the image dimensions (height, width, channels)
image = pixel_data.reshape((height, width, 4))  # 4 channels for BGRA

# Convert from BGRA to RGBA for better compatibility in Python image libraries
image_rgba = image[..., [2, 1, 0, 3]]  # Swap B and R channels

# Save the image
image_rgba = Image.fromarray(image_rgba, 'RGBA')
image_rgba.save('framebuffer.png')

print("Framebuffer image saved as framebuffer.png")