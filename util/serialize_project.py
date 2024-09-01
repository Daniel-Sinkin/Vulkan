"""
Utility that serializes the entire project (all .cpp in src and .h in include) as one large string
with filenames included.

Intention is to allow for faster turnaround speed when using LLMs, at least once the project is small,
once it gets bigger I'd need some kind of filtering mechanism, maybe only including function names,
filtering for specific regex matching names and the like.

This is closely related to some CLI tools I've developed for exploring and parsing the AST of python projects

https://github.com/Daniel-Sinkin/ds_util/
"""

import argparse
import os
import subprocess

import pyperclip
import tiktoken
from termcolor import colored


def get_project_structure() -> str:
    try:
        result = subprocess.run(["tree", "."], capture_output=True, text=True)
        return result.stdout
    except FileNotFoundError:
        # If tree command is not available, fallback to using os.walk for structure
        structure = ""
        for root, dirs, files in os.walk("."):
            level = root.replace(".", "").count(os.sep)
            indent = " " * 4 * level
            structure += f"{indent}{os.path.basename(root)}/\n"
            sub_indent = " " * 4 * (level + 1)
            for file in files:
                structure += f"{sub_indent}{file}\n"
        return structure


def serialize_files(directory, extension) -> str:
    serialized_content = f"###\n# {directory.upper()} FOLDER\n###\n\n"
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(extension):
                filepath = os.path.join(root, file)
                with open(filepath, "r") as f:
                    content = f.read()
                    serialized_content += f"###\n# {file}\n###\n"
                    serialized_content += content + "\n\n"
    return serialized_content


def list_shaders_and_content(folder) -> str:
    serialized_content = f"###\n# {folder.upper()} FOLDER\n###\n\n"
    for file in os.listdir(folder):
        filepath = os.path.join(folder, file)
        if os.path.isfile(filepath):
            serialized_content += f"###\n# {file}\n###\n"
            with open(filepath, "r") as f:
                content = f.read()
                serialized_content += content + "\n\n"
    return serialized_content


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Serialize project files and optionally list shader files."
    )
    parser.add_argument(
        "--shaders",
        action="store_true",
        help="Append shader files' contents to the serialized output",
    )
    args = parser.parse_args()

    src_dir = "src"
    include_dir = "include"
    shaders_dir = "shaders"

    project_structure = get_project_structure()

    serialized_string = "###\n# Project structure\n###\n"
    serialized_string += project_structure + "\n\n"

    serialized_string += serialize_files(src_dir, ".cpp")
    serialized_string += serialize_files(include_dir, ".h")

    if args.shaders:
        serialized_string += list_shaders_and_content(shaders_dir)

    pyperclip.copy(serialized_string)

    num_lines = len(serialized_string.splitlines())

    encoding = tiktoken.encoding_for_model("gpt-4")
    num_tokens = len(encoding.encode(serialized_string))

    token_limit = 8192
    if num_tokens > token_limit / 2:
        print(
            colored(
                f"Warning: You have reached {num_tokens} tokens out of {token_limit}. This is more than 50% of the limit.",
                "red",
            )
        )
    else:
        print(f"Number of tokens: {num_tokens}")

    print(f"Number of lines copied to clipboard: {num_lines}")


if __name__ == "__main__":
    main()
