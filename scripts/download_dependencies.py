import os
import json
import hashlib
import requests
from pathlib import Path
from typing import Dict

"""
Module for downloading and verifying external dependencies and assets by checking 
their MD5 hashes. The script ensures that necessary files are downloaded, stored 
locally, and verified against their expected hash values.
"""

dependancy_json_encoding = """
{
    "dependencies": {
        "stb_image": {
            "url": "https://raw.githubusercontent.com/nothings/stb/013ac3b/stb_image.h",
            "expected_md5": "27932e6fb3a2f26aee2fc33f2cb4e696",
            "local_file": "./external/stb_image.h"
        },
        "tiny_obj_loader": {
            "url": "https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/7b3ba0b/tiny_obj_loader.h",
            "expected_md5": "3226a14e4a4c3a51ddbfa87bd6240f6b",
            "local_file": "./external/tiny_obj_loader.h"
        }
    },
    "assets": {
        "texture": {
            "url": "https://vulkan-tutorial.com/images/texture.jpg",
            "expected_md5": "4e60ee5adc9dd77c029faeb96fd113c0",
            "local_file": "./textures/texture.jpg"
        }
    }
}
"""

data = json.loads(dependancy_json_encoding)

EXTERNAL_INCLUDE_DIR = Path("./external")
TEXTURE_FOLDER = Path("./textures")

EXTERNAL_INCLUDE_DIR.mkdir(parents=True, exist_ok=True)
TEXTURE_FOLDER.mkdir(parents=True, exist_ok=True)


def calculate_md5(file_path: Path) -> str:
    """
    Calculates the MD5 hash of a file.

    Args:
        file_path (Path): The path to the file to hash.

    Returns:
        str: The computed MD5 hash as a hexadecimal string.
    """
    hash_md5 = hashlib.md5()
    with file_path.open("rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()


def download_file(url: str, local_file: Path) -> bool:
    """
    Downloads a file from a given URL and saves it locally.

    Args:
        url (str): The URL to download the file from.
        local_file (Path): The path to save the downloaded file.

    Returns:
        bool: True if the download was successful, False otherwise.
    """
    try:
        print(f"\t\tDownloading {local_file.name}...")
        response = requests.get(url)
        response.raise_for_status()
        with local_file.open("wb") as f:
            f.write(response.content)
        return True
    except requests.RequestException as e:
        print(f"\t\tError downloading {local_file.name}: {e}")
        return False


def download_and_verify(file_info: Dict[str, str]) -> None:
    """
    Downloads a file and verifies its MD5 hash. If the file already exists and 
    matches the expected hash, no download is performed. If the hash does not match,
    the file is re-downloaded.

    Args:
        file_info (Dict[str, str]): A dictionary containing file metadata, including:
            - "url": The URL to download the file from.
            - "expected_md5": The expected MD5 hash of the file.
            - "local_file": The local path to save the downloaded file.
    """
    url = file_info["url"]
    local_file = Path(file_info["local_file"])
    expected_md5 = file_info["expected_md5"]

    if local_file.exists():
        local_md5 = calculate_md5(local_file)
        if local_md5 == expected_md5:
            print(f"\t\t{local_file.name} is already downloaded and the MD5 hash matches.")
            return
        else:
            print(f"\t\t{local_file.name} is downloaded but the MD5 hash does not match. Downloading again...")
            local_file.unlink()

    if download_file(url, local_file):
        local_md5 = calculate_md5(local_file)
        if local_md5 == expected_md5:
            print(f"\t\t{local_file.name} downloaded and verified successfully.")
        else:
            print(f"\t\tMD5 hash mismatch after downloading {local_file.name}. Exiting.")
            exit(1)
    else:
        print(f"\t\tFailed to download {local_file.name}. Exiting.")
        exit(1)


def main() -> None:
    """
    Main function that orchestrates the downloading and verifying of dependencies
    and assets as specified in the JSON-encoded data.
    """
    print("Starting to download missing dependencies and assets.")
    print("\tDependencies")
    for file_key, file_info in data["dependencies"].items():
        download_and_verify(file_info)

    print("\tAssets")
    for asset_key, asset_info in data["assets"].items():
        download_and_verify(asset_info)
    print("Finished all downloads.")


if __name__ == "__main__":
    main()