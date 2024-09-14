import numpy as np
import matplotlib.pyplot as plt

# File path to the saved binary file
file_path = "ascii_texture_atlas.bin"

# Define the dimensions of the atlas
atlas_width = 512
atlas_height = 512

# Load the binary data into a numpy array
with open(file_path, "rb") as f:
    atlas_data = np.frombuffer(f.read(), dtype=np.uint8)

# Reshape the array to match the texture dimensions
atlas_data = atlas_data.reshape((atlas_height, atlas_width))

# Display the texture atlas using matplotlib
plt.imshow(atlas_data, cmap="gray")
plt.title("ASCII Texture Atlas")
plt.axis("off")  # Hide the axes
plt.show()