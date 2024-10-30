#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_msg() {
    echo -e "${2}${1}${NC}"
}

# Function to check if a command exists
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_msg "Error: $1 is not installed. Please install it first." "$RED"
        exit 1
    fi
}

# Function to handle errors
handle_error() {
    print_msg "Error: Build failed!" "$RED"
    exit 1
}

# Check for required tools
check_command gcc
check_command make
check_command pkg-config

# Parse command line arguments
BUILD_TYPE="make"
CLEAN=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --cmake)
            check_command cmake
            BUILD_TYPE="cmake"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --help)
            echo "Usage: ./build.sh [OPTIONS]"
            echo "Options:"
            echo "  --cmake    Use CMake instead of Make"
            echo "  --clean    Clean build files before building"
            echo "  --help     Show this help message"
            exit 0
            ;;
        *)
            print_msg "Unknown option: $1" "$RED"
            exit 1
            ;;
    esac
done

# Create build directory if it doesn't exist
mkdir -p build

if [ "$CLEAN" = true ]; then
    print_msg "Cleaning build files..." "$YELLOW"
    if [ "$BUILD_TYPE" = "cmake" ]; then
        rm -rf build/*
    else
        make clean
    fi
fi

# Build the project
print_msg "Building XOpenPlayer using $BUILD_TYPE..." "$YELLOW"

if [ "$BUILD_TYPE" = "cmake" ]; then
    cd build || handle_error
    cmake .. || handle_error
    make || handle_error
    cd ..
else
    make || handle_error
fi

# Check if build was successful
if [ $? -eq 0 ]; then
    print_msg "Build successful!" "$GREEN"
    if [ "$BUILD_TYPE" = "cmake" ]; then
        print_msg "Executable located in: build/bin/XOpenPlayer" "$GREEN"
    else
        print_msg "Executable located in: bin/XOpenPlayer" "$GREEN"
    fi
else
    handle_error
fi