#!/usr/bin/env python3

import os
import argparse
import sys

# Set up command line argument parsing
parser = argparse.ArgumentParser(description='Process the image file.')
parser.add_argument('idb_file_path', metavar='idb_file_path', type=str, help='Path to the image file')

# Parse command line arguments
args = parser.parse_args()

# File path
idb_file_path = args.idb_file_path
double_idb_file_path = idb_file_path + '_double'

# Check if the file exists
if not os.path.exists(idb_file_path):
    print(f"File {idb_file_path} does not exist.", file=sys.stderr)
    sys.exit(1)

# Get the file size
file_size = os.path.getsize(idb_file_path)

# Check if the file size is greater than 64KB
if file_size > 64 * 1024:
    print("Error: File size exceeds 64KB.", file=sys.stderr)
    sys.exit(1)

# Extend the file to 64KB, filling the rest with zeros
with open(idb_file_path, 'rb') as f:
    data = f.read()

# Calculate the amount of zeros needed for padding
padding_size = 64 * 1024 - len(data)
padding = b'\x00' * padding_size

# Write back to the file
with open(idb_file_path, 'wb') as f:
    f.write(data + padding)

# Read the extended file content and write it into the new file twice
with open(idb_file_path, 'rb') as f:
    data = f.read()

with open(double_idb_file_path, 'wb') as f:
    f.write(data + data)

# Delete the original file
try:
    os.remove(idb_file_path)
except OSError as e:
    print(f"Error: {e.strerror} - {idb_file_path}", file=sys.stderr)
    sys.exit(1)

# Rename the double file to the original file name
try:
    os.rename(double_idb_file_path, idb_file_path)
except OSError as e:
    print(f"Error: {e.strerror} - {double_idb_file_path}", file=sys.stderr)
    sys.exit(1)

#print("Processing complete and file renamed.")
