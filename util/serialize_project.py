"""
This script generates a serialized content output from a project's `src`, `include`, and
CMakeLists.txt files. It counts the lines, generates a tree structure of the project,
copies the serialized output to the clipboard, and calculates the number of tokens for
use with OpenAI models.
"""

import subprocess
from pathlib import Path

import pyperclip
import tiktoken

TOKENLIMIT = 6000

# Global project root as pathlib object
project_root = Path("..").resolve()


def get_file_content(filepath):
    with open(filepath, "r") as file:
        return file.read()


def count_lines(content):
    return len(content.splitlines())


def generate_serialized_content():
    """Generate the serialized content from the source and header files."""
    output = []
    file_list = []
    total_lines = 0

    # Process src folder
    src_path = project_root.joinpath("src")
    if src_path.exists() and src_path.is_dir():
        file_list.append("src/")
        output.append("###\n# SRC\n###\n")
        for cpp_file in src_path.glob("*.cpp"):
            file_list.append(f"    {cpp_file.name}")
            content = get_file_content(cpp_file)
            total_lines += count_lines(content)
            output.append(f"# {cpp_file.name}\n")
            output.append(content)
            output.append("\n")

    # Process include folder
    include_path = project_root.joinpath("include")
    if include_path.exists() and include_path.is_dir():
        file_list.append("include/")
        output.append("###\n# INCLUDE\n###\n")
        for h_file in include_path.glob("*.h"):
            file_list.append(f"    {h_file.name}")
            content = get_file_content(h_file)
            total_lines += count_lines(content)
            output.append(f"# {h_file.name}\n")
            output.append(content)
            output.append("\n")

    # Process CMakeLists.txt
    cmake_file = project_root.joinpath("CMakeLists.txt")
    if cmake_file.exists():
        file_list.append("CMakeLists.txt")
        output.append("###\n# CMakeLists.txt\n###\n")
        content = get_file_content(cmake_file)
        total_lines += count_lines(content)
        output.append(content)
        output.append("\n")

    # Generate project tree
    output.append("###\n# Project Tree\n###\n")
    tree_command = ["tree", str(project_root)]
    project_tree = subprocess.run(tree_command, stdout=subprocess.PIPE, text=True)
    output.append(project_tree.stdout)

    return "\n".join(output), file_list, total_lines


def count_tokens(text):
    """Count the number of tokens in the text using tiktoken."""
    # Use 'cl100k_base' for GPT-4 encoding or 'p50k_base' for GPT-3.5
    encoding = tiktoken.get_encoding("cl100k_base")
    tokens = encoding.encode(text)
    return len(tokens)


def main():
    serialized_content, file_list, total_lines = generate_serialized_content()

    # Copy to clipboard
    pyperclip.copy(serialized_content)
    print("Serialized content copied to clipboard.")

    # Print the list of files considered
    print("\nFiles Considered:")
    for file in file_list:
        print(file)

    # Print the total number of lines copied
    print(f"\nTotal lines copied: {total_lines}")

    # Calculate and print the number of tokens
    token_count = count_tokens(serialized_content)
    print(f"Total tokens: {token_count}")

    if token_count > TOKENLIMIT:
        print(
            f"Warning: The content exceeds {TOKENLIMIT} tokens, which may not fit within a single OpenAI GPT-3.5/4 request."
        )


if __name__ == "__main__":
    main()
