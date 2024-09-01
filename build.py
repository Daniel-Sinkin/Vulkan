import argparse
import os
import platform
import subprocess
import sys
from pathlib import Path


def command_exists(command: str) -> bool:
    return (
        subprocess.call(
            f"command -v {command}",
            shell=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        == 0
    )


def check_required_tools() -> None:
    tools = {
        "cmake": "CMake is not installed. Please install CMake and try again.",
        "make": "Make is not installed. Please install Make and try again.",
        "glslc": "glslc (Vulkan shader compiler) is not installed. Please install the Vulkan SDK and try again.",
    }

    for tool, error_message in tools.items():
        if not command_exists(tool):
            sys.exit(f"Error: {error_message}")


def run_command(command: str, error_message: str) -> None:
    if subprocess.call(command, shell=True) != 0:
        sys.exit(f"Error: {error_message}")


def list_shader_files() -> None:
    shader_dir = Path("shaders")
    if not shader_dir.exists():
        print("No shaders directory found.")
        return

    print("Listing shader files in the shaders directory:")
    for item in shader_dir.iterdir():
        if item.is_file():
            print(f"  {item.name}")


def compile_shaders() -> None:
    shader_dir = Path("shaders")
    compiled_dir = shader_dir / "compiled"
    compiled_dir.mkdir(parents=True, exist_ok=True)

    shaders = list(shader_dir.glob("*.vert")) + list(shader_dir.glob("*.frag"))

    for shader in shaders:
        name = shader.stem
        extension = shader.suffix
        output = compiled_dir / f"{name}{extension}.spv"
        run_command(
            f"glslc {shader} -o {output}", f"Failed to compile shader {shader.name}"
        )
        print(f"Compiled: {shader.name} -> {output.name}")

    print("All shaders compiled successfully!")


def main(run_after_build: bool, list_shaders: bool) -> None:
    if platform.system() != "Darwin":
        print("Warning: This project is only tested on macOS.")

    check_required_tools()

    if list_shaders:
        list_shader_files()

    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)
    os.chdir(build_dir)

    run_command("cmake ..", "CMake configuration failed")
    run_command("make", "Build failed")

    os.chdir("..")

    compile_shaders()

    if run_after_build:
        run_command("./build/VulkanEngine", "Failed to run the executable")
        print("Program executed successfully!")

    print("Build completed successfully!")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Build and optionally run the VulkanEngine project."
    )
    parser.add_argument(
        "--run", action="store_true", help="Run the program after building"
    )
    parser.add_argument(
        "--shaders",
        action="store_true",
        help="List shader files in the shaders directory",
    )
    args = parser.parse_args()

    main(args.run, args.shaders)
