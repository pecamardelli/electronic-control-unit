#!/bin/bash

# Check if a directory is provided as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

# Directory to search
TARGET_DIR=$1

# Find all .c files and rename them to .cpp
find "$TARGET_DIR" -type f -name "*.c" | while read -r file; do
    # Get the file without the extension
    base="${file%.c}"
    # Rename to .cpp
    mv "$file" "${base}.cpp"
    echo "Renamed: $file -> ${base}.cpp"
done

echo "All .c files have been renamed to .cpp."
