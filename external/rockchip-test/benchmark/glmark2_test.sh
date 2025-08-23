#!/bin/bash

# Define the path to the script relative to the current script's location
SCRIPT_PATH=" ${CURRENT_DIR}/../gpu/gpu_test.sh &"

# Check if the script exists and is executable
if [ -x "$SCRIPT_PATH" ]; then
    echo "Executing the GPU test script..."
    # Execute the script
    $SCRIPT_PATH
else
    echo "Error: The script '$SCRIPT_PATH' does not exist or is not executable."
    exit 1
fi
