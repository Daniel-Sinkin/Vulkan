import subprocess
from pathlib import Path

import pyperclip
import tiktoken

TOKENLIMIT = 10_000


def get_file_content(filepath):
    """Read the content of a file and return it as a string."""
    with open(filepath, "r") as file:
        return file.read()


def count_lines(content):
    """Count the number of lines in the content."""
    return len(content.splitlines())


def process_directory(output, file_list, total_lines, directory, extension, header):
    """Process a directory, appending its content to the output."""
    if directory.exists() and directory.is_dir():
        file_list.append(f"{directory.name}/")
        output.append(f"###\n# {header}\n###\n")
        for file in directory.glob(f"*{extension}"):
            file_list.append(f"    {file.name}")
            content = get_file_content(file)
            total_lines += count_lines(content)
            output.append(f"# {file.name}\n")
            output.append(content)
            output.append("\n")
    return total_lines


def generate_serialized_content(project_root):
    """Generate the serialized content from the source and header files."""
    output = []
    file_list = []
    total_lines = 0

    # Process src folder
    src_path = project_root.joinpath("src")
    total_lines = process_directory(
        output, file_list, total_lines, src_path, ".cpp", "SRC"
    )

    # Process include folder
    include_path = project_root.joinpath("include")
    total_lines = process_directory(
        output, file_list, total_lines, include_path, ".h", "INCLUDE"
    )

    # Process src/datastructures folder
    datastructures_path = src_path.joinpath("datastructures")
    total_lines = process_directory(
        output,
        file_list,
        total_lines,
        datastructures_path,
        ".cpp",
        "SRC/DATASTRUCTURES",
    )
    total_lines = process_directory(
        output,
        file_list,
        total_lines,
        datastructures_path,
        ".h",
        "INCLUDE/DATASTRUCTURES",
    )

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
    # Assuming this script is in the 'util' folder, derive the root directory
    project_root = Path(__file__).resolve().parent.parent

    serialized_content, file_list, total_lines = generate_serialized_content(
        project_root
    )

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
