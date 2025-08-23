#!/usr/bin/env python3

import sys

def generate_zero_filled_file(filename, size_kb):
    size_bytes = size_kb * 1024  # Convert size from KB to bytes
    with open(filename, 'wb') as file:
        file.write(bytearray(size_bytes))  # Write `size_bytes` zeros to the file

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./mkmisc.py <size_kb>")
        sys.exit(1)

    size_kb = int(sys.argv[1])
    generate_zero_filled_file('Image/misc.img', size_kb)

