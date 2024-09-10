import os
import sys
import subprocess
import time
from pathlib import Path

def run_command(command):
    """Run a shell command and exit on failure."""
    result = subprocess.run(command, shell=True)
    if result.returncode != 0:
        print(f"Error running command: {command}. Exiting.")
        exit(1)

def main(build_type="Debug"):
    # Record the start time with nanosecond precision
    start_time = time.perf_counter()

    # Change to the script directory
    script_dir = Path(__file__).parent.resolve()
    os.chdir(script_dir)

    # Call the download dependencies Python script
    run_command("python3 scripts/download_dependencies.py")

    # Compile shaders
    run_command("./scripts/compile_shaders.sh")

    # Clean and prepare the build directory
    build_dir = script_dir.joinpath("build")
    if build_dir.exists():
        subprocess.run(f"rm -r {build_dir}", shell=True)
    build_dir.mkdir()

    os.chdir(build_dir)

    # Determine build type
    if build_type.lower() == "release":
        cmake_build_type = "Release"
    else:
        cmake_build_type = "Debug"

    # Configure the project using CMake with Ninja as the generator
    run_command(f"cmake -G Ninja -DCMAKE_BUILD_TYPE={cmake_build_type} ..")

    # Build the project using ninja (automatically detects the number of cores)
    run_command("ninja")

    # Record the end time with nanosecond precision
    end_time = time.perf_counter()

    # Calculate the elapsed time
    elapsed_time = end_time - start_time

    # Print the elapsed time rounded to 2 decimal places
    print(f"Build completed in {elapsed_time:.2f} seconds.")

if __name__ == "__main__":
    build_type_arg = "Debug" if len(sys.argv) < 2 else sys.argv[1].lstrip("--")
    main(build_type=build_type_arg)