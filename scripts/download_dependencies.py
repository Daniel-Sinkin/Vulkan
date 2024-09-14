"""
Handles dependency downloads based on the accompanying `dependency_data.json` file. If a md5
is availiable then the file gets checked against it, replacing it if its missing.

If no md5 hash is availiable then it gets computed and written into the json file.

zip files get unzipped automatically with the pattern $1.zip -> $1, in particular this means
that the expect the zip to already contain the correct file fileextension. For example
the `chalet.obj.zip` file which gets unzipped into `chalet.obj`.
"""

import os  
import json
import hashlib
import requests
import zipfile
from pathlib import Path
from typing import cast, TypedDict
import subprocess  # Using subprocess to call FontForge

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
    with json_file.open("r") as f:
        data = json.load(f)
    return cast(Data, data)

def save_data_to_json(data: Data, json_file: Path) -> None:
    with json_file.open("w") as f:
        json.dump(data, f, indent=4)

def calculate_md5(file_path: Path) -> str:
    hash_md5 = hashlib.md5()
    with file_path.open("rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def download_file(url: str, local_file: Path) -> bool:
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
            print(f"\t\tMD5 hash for {local_file.name} is missing in the JSON. Calculating and saving it...")
            data[section][file_key]["expected_md5"] = local_md5
            save_data_to_json(data, DATA_FILE)
            print(f"\t\tMD5 hash for {local_file.name} computed and saved to {DATA_FILE}.")
    else:
        print(f"\t\tFailed to download {local_file.name}. Exiting.")
        exit(1)

def unzip_file_if_needed(zip_file: Path) -> None:
    if not zip_file.suffix == ".zip":
        return

    output_file = zip_file.with_suffix('')  # Remove the .zip suffix
    if output_file.exists():
        print(f"\t\t{output_file.name} already exists, skipping unzip.")
        return

    print(f"\t\tUnzipping {zip_file.name} to {output_file.name}...")
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        zip_ref.extractall(zip_file.parent)
    print(f"\t\tUnzipped {zip_file.name} successfully.")

def convert_otf_to_ttf(otf_file: Path) -> None:
    ttf_file = otf_file.with_suffix('.ttf')
    
    if ttf_file.exists():
        print(f"\t\t{ttf_file.name} already exists, skipping conversion.")
        return
    
    print(f"\t\tConverting {otf_file.name} to {ttf_file.name}...")
    try:
        # Use FontForge command-line tool via subprocess
        subprocess.run(
            ["fontforge", "-lang=ff", "-c", f"Open('{otf_file}'); Generate('{ttf_file}');"],
            check=True
        )
        print(f"\t\tConverted {otf_file.name} to {ttf_file.name}.")
    except subprocess.CalledProcessError as e:
        print(f"\t\tError converting {otf_file.name} to TTF: {e}")

def handle_otf_file(local_file: Path) -> None:
    if local_file.suffix == ".otf":
        convert_otf_to_ttf(local_file)

def main() -> None:
    if not DATA_FILE.exists():
        print(f"Data file {DATA_FILE} not found. Exiting.")
        exit(1)

    data = load_data_from_json(DATA_FILE)

    print("Starting to download missing dependencies and assets.")
    print("\tDependencies")
    for file_key, file_info in data["dependencies"].items():
        download_and_verify(file_info, data, file_key, "dependencies")

        local_file = Path(file_info["local_file"])
        
        # Check for .zip files and unzip if needed
        unzip_file_if_needed(local_file)

        # Check for .otf files and convert them to .ttf if needed
        handle_otf_file(local_file)

    print("\tAssets")
    for asset_key, asset_info in data["assets"].items():
        download_and_verify(asset_info, data, asset_key, "assets")

        local_file = Path(asset_info["local_file"])

        # Check for .zip files and unzip if needed
        unzip_file_if_needed(local_file)

        # Check for .otf files and convert them to .ttf if needed
        handle_otf_file(local_file)

    print("Finished all downloads and conversions.")

if __name__ == "__main__":
    main()