#!/bin/bash

check_macos() {
    if [[ "$(uname)" != "Darwin" ]]; then
        echo "This script only supports macOS."
        exit 1
    fi
}

install_xcode_tools() {
    if ! xcode-select -p &> /dev/null; then
        echo "Xcode Command Line Tools not found. Installing..."
        xcode-select --install

        # Wait until the installation is confirmed before proceeding
        until xcode-select -p &> /dev/null; do
            echo "Waiting for Xcode Command Line Tools to be installed..."
            sleep 5
        done

        echo "Xcode Command Line Tools have been installed."
    else
        echo "Xcode Command Line Tools are already installed."
    fi
}

install_homebrew() {
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        echo "Homebrew is already installed."
    fi
}

install_packages() {
    echo "Updating Homebrew..."
    brew update

    echo "Installing CMake..."
    brew install cmake

    echo "Installing GLFW..."
    brew install glfw

    echo "Installing GLEW..."
    brew install glew

    echo "All necessary packages have been installed."
}

# Main script execution
check_macos
install_xcode_tools
install_homebrew
install_packages

echo "Setup complete. You are ready to run this project!"