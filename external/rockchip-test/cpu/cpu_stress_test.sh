#!/bin/bash

# Specify the duration of the stress test in seconds
DURATION=86400 # 24 hours in seconds
# Specify the number of CPU cores to stress
CPU_CORES=$(nproc) # This will use all available CPU cores

# Function to perform stress test using stress tool
stress_test_stress() {
    echo "Starting 24-hour CPU stress test with stress tool on $CPU_CORES cores."
    stress --cpu $CPU_CORES --timeout $DURATION
}

# Function to perform stress test using stress-ng tool
stress_test_stress_ng() {
    echo "Starting 24-hour CPU stress test with stress-ng tool on $CPU_CORES cores."
    stress-ng --cpu $CPU_CORES --timeout $DURATION
}

# Check if stress-ng is available
if command -v stress-ng >/dev/null 2>&1; then
    stress_test_stress_ng
elif command -v stress >/dev/null 2>&1; then
    stress_test_stress
else
    echo "Neither stress nor stress-ng tool is installed. Please install one of them to proceed."
    exit 1
fi

echo "24-hour CPU stress test completed."
