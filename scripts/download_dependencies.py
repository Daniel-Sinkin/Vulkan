import os
import json
import hashlib
import requests
from pathlib import Path
from typing import cast, TypedDict

class Dependency(TypedDict):
    url: str
    expected_md5: str
    local_file: str

class Data(TypedDict):
    dependencies: dict[str, Dependency]
    assets: dict[str, Dependency]

EXTERNAL_INCLUDE_DIR = Path("./external")
ASSETS_FOLDER = Path("./assets")
TEXTURE_FOLDER = ASSETS_FOLDER.joinpath("textures")
MODELS_FOLDER = ASSETS_FOLDER.joinpath("models")
SCRIPTS_FOLDER = Path("./scripts")
DATA_FILE = SCRIPTS_FOLDER.joinpath("dependency_data.json")

EXTERNAL_INCLUDE_DIR.mkdir(parents=True, exist_ok=True)
TEXTURE_FOLDER.mkdir(parents=True, exist_ok=True)
MODELS_FOLDER.mkdir(parents=True, exist_ok=True)

def load_data_from_json(json_file: Path) -> Data:
    """
    Loads the data from a JSON file.

    Args:
        json_file (Path): Path to the JSON file containing the data.

    Returns:
        Data: Parsed data from the JSON file.
    """
    with json_file.open("r") as f:
        data = json.load(f)
    return cast(Data, data)

def save_data_to_json(data: Data, json_file: Path) -> None:
    """
    Saves the data to a JSON file.

    Args:
        data (Data): Data to save.
        json_file (Path): Path to the JSON file.
    """
    with json_file.open("w") as f:
        json.dump(data, f, indent=4)

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


def download_and_verify(file_info: dict[str, str], data: Data, file_key: str, section: str) -> None:
    """
    Downloads a file and verifies its MD5 hash. If the file already exists and 
    matches the expected hash, no download is performed. If the hash does not match,
    the file is re-downloaded. If the expected MD5 is missing, it computes the hash 
    after download and writes it back to the JSON file.

    Args:
        file_info (dict[str, str]): A dictionary containing file metadata, including:
            - "url": The URL to download the file from.
            - "expected_md5": The expected MD5 hash of the file.
            - "local_file": The local path to save the downloaded file.
        data (Data): The full data structure to update the JSON file.
        file_key (str): The key of the current file in the data dictionary.
        section (str): The section ("dependencies" or "assets") where the file is located.
    """
    url = file_info["url"]
    local_file = Path(file_info["local_file"])
    expected_md5 = file_info.get("expected_md5")

    if local_file.exists() and expected_md5:
        local_md5 = calculate_md5(local_file)
        if local_md5 == expected_md5:
            print(f"\t\t{local_file.name} is already downloaded and the MD5 hash matches.")
            return
        else:
            print(f"\t\t{local_file.name} is downloaded but the MD5 hash does not match. Downloading again...")
            local_file.unlink()

    if download_file(url, local_file):
        local_md5 = calculate_md5(local_file)
        if expected_md5:
            if local_md5 == expected_md5:
                print(f"\t\t{local_file.name} downloaded and verified successfully.")
            else:
                print(f"\t\tMD5 hash mismatch after downloading {local_file.name}. Exiting.")
                exit(1)
        else:
            # If the MD5 hash was missing in the JSON, update the file and save the data
            print(f"\t\tMD5 hash for {local_file.name} is missing in the JSON. Calculating and saving it...")
            data[section][file_key]["expected_md5"] = local_md5
            save_data_to_json(data, DATA_FILE)
            print(f"\t\tMD5 hash for {local_file.name} computed and saved to {DATA_FILE}.")
    else:
        print(f"\t\tFailed to download {local_file.name}. Exiting.")
        exit(1)


def main() -> None:
    """
    Main function that orchestrates the downloading and verifying of dependencies
    and assets as specified in the JSON-encoded data.
    """
    if not DATA_FILE.exists():
        print(f"Data file {DATA_FILE} not found. Exiting.")
        exit(1)

    # Load data from JSON file
    data = load_data_from_json(DATA_FILE)
    
    print("Starting to download missing dependencies and assets.")
    print("\tDependencies")
    for file_key, file_info in data["dependencies"].items():
        download_and_verify(file_info, data, file_key, "dependencies")

    print("\tAssets")
    for asset_key, asset_info in data["assets"].items():
        download_and_verify(asset_info, data, asset_key, "assets")
    
    print("Finished all downloads.")


if __name__ == "__main__":
    main()