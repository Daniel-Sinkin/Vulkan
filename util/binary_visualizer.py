import numpy as np
from PIL import Image

# Load the binary file
width = 800  # Set the correct width of the swap chain image
height = 600  # Set the correct height of the swap chain image

# Assuming 4 channels (BGRA), 8-bit per channel
pixel_data = np.fromfile("framebuffer.bin", dtype=np.uint8)

# Reshape to match the image dimensions (height, width, channels)
image = pixel_data.reshape((height, width, 4))  # 4 channels for BGRA

# Split the image into even and odd rows
even_rows = image[::2, :, :]  # Even rows
odd_rows = image[1::2, :, :]  # Odd rows

# Create a new array to combine both even and odd rows
combined_image = np.empty((height, width, 4), dtype=np.uint8)

# Fill even rows and odd rows into the combined image
combined_image[::2, :, :] = even_rows
combined_image[1::2, :, :] = odd_rows

# Optional: Convert from BGRA to RGBA for better compatibility in Python image libraries
combined_image_rgba = combined_image[..., [2, 1, 0, 3]]  # Swap B and R channels

# Convert NumPy array to PIL Image and save it
image_rgba = Image.fromarray(combined_image_rgba, 'RGBA')
image_rgba.save('framebuffer_corrected.png')