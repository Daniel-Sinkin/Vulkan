import numpy as np
from PIL import Image
from pathlib import Path
import multiprocessing as mp
import time

# Define the paths
raw_folder = Path("./Screencaps/Raw")
converted_folder = Path("./Screencaps/RawConverted")

# Create the converted folder if it doesn't exist
converted_folder.mkdir(parents=True, exist_ok=True)

def process_files(files):
    """
    Load and process all files assigned to this process.
    """
    for input_file in files:
        start_time = time.perf_counter()  # Start timing the conversion

        try:
            # Define the corresponding output file path
            output_file = converted_folder.joinpath(input_file.stem + ".png")

            # Read the binary data from the file
            with open(input_file, "rb") as f:
                # Read the width (first 4 bytes) and height (next 4 bytes) as 32-bit integers
                width = np.frombuffer(f.read(4), dtype=np.uint32)[0]
                height = np.frombuffer(f.read(4), dtype=np.uint32)[0]

                # Read the remaining data as pixel data (assuming 4 channels per pixel)
                framebuffer = np.frombuffer(f.read(), dtype=np.uint8)

            # Ensure the size matches the expected dimensions
            channels = 4  # Assuming 4 channels: Red, Green, Blue, Alpha
            expected_size = width * height * channels

            if framebuffer.size != expected_size:
                raise ValueError(f"Framebuffer size mismatch in {input_file}. "
                                 f"Expected {expected_size} bytes, got {framebuffer.size} bytes")

            # Reshape the flat array to a 3D array (height, width, channels)
            image_array = framebuffer.reshape((height, width, channels))

            # Swap the R and B channels to go from SRGB to BGR
            image_array = image_array[..., [2, 1, 0, 3]]  # Swap B and R channels

            # Create a PIL image from the modified numpy array
            image = Image.fromarray(image_array, "RGBA")

            # Save the image as a PNG file
            image.save(output_file, compress_level=0)  # No compression for speed

            # Measure the time taken for this render
            end_time = time.perf_counter()
            render_time = end_time - start_time  # Render time in seconds

            # Print aligned output with render time
            print(f"Converted {str(input_file):<40} -> {str(output_file):<40} ({render_time:.2f}s)")

        except Exception as e:
            print(f"Failed to convert {input_file}: {e}")

def chunkify(lst, n):
    """
    Split the list into n chunks.
    """
    return [lst[i::n] for i in range(n)]

if __name__ == "__main__":
    # Start the overall time measurement
    overall_start_time = time.perf_counter()

    # Get all the .bin files from the raw folder
    bin_files = list(raw_folder.glob("*.bin"))

    # Determine the number of processes (number of CPU cores)
    pool_size = mp.cpu_count()

    # Split the files into chunks, one chunk per process
    chunks = chunkify(bin_files, pool_size)

    # Use multiprocessing to parallelize the conversion
    with mp.Pool(pool_size) as pool:
        pool.map(process_files, chunks)

    # Measure the total elapsed time
    overall_end_time = time.perf_counter()
    total_time = overall_end_time - overall_start_time

    # Calculate images per second
    images_per_second = len(bin_files) / total_time if total_time > 0 else 0

    # Print the summary
    print("\nSummary:")
    print(f"Total files converted: {len(bin_files)}")
    print(f"Number of cores used : {pool_size}")
    print(f"Total time taken     : {total_time:.2f} seconds")
    print(f"Images per second    : {images_per_second:.2f}")