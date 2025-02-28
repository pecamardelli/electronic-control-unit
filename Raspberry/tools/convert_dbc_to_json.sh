#!/bin/bash

# Check if pipx and canmatrix are installed
if ! command -v pipx &> /dev/null; then
    echo "Error: pipx is not installed. Install it using: sudo apt install pipx && pipx ensurepath"
    exit 1
fi

if ! pipx list | grep -q "canmatrix"; then
    echo "Error: canmatrix is not installed in pipx. Installing now..."
    pipx install canmatrix
fi

# Check if input file is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_dbc_file>"
    exit 1
fi

INPUT_DBC="$1"
OUTPUT_JSON="${INPUT_DBC%.dbc}.json"

# Convert DBC to JSON using canconvert from pipx
echo "Converting $INPUT_DBC to $OUTPUT_JSON..."
canconvert -f json "$INPUT_DBC" "$OUTPUT_JSON"

# Check if conversion was successful
if [ $? -eq 0 ]; then
    echo "Conversion successful! JSON file saved as $OUTPUT_JSON"
else
    echo "Error: Conversion failed!"
    exit 1
fi
