#!/usr/bin/env python3

import sys
import os
import struct
import ctypes
import subprocess

def compile_js_hash_library():
    source_file = '../tools/js_hash/js_hash.c'
    output_file = 'libjshash.so'
    compile_command = ['gcc', '-shared', '-o', output_file, '-fPIC', source_file]

    try:
        result = subprocess.run(compile_command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #print(f"Compilation succeeded: {result.stdout.decode('utf-8')}")
    except subprocess.CalledProcessError as e:
        print(f"Compilation failed: {e.stderr.decode('utf-8')}")
        sys.exit(1)

# Compile the C library first
compile_js_hash_library()

# Load the shared library
libjshash = ctypes.CDLL('./libjshash.so')

# Define the JS_Hash function signature
libjshash.JS_Hash.argtypes = (ctypes.POINTER(ctypes.c_uint8), ctypes.c_size_t)
libjshash.JS_Hash.restype = ctypes.c_uint32

def JS_Hash_c(buf):
    buf_array = (ctypes.c_uint8 * len(buf)).from_buffer_copy(buf)
    return libjshash.JS_Hash(buf_array, len(buf))

# JS Hash Function
def JS_Hash(buf):
    hash = 0x47C6A7E6
    for b in buf:
        hash ^= ((hash << 5) + b + (hash >> 2))
    return hash


def create_firmware_update(rk2118_bin_path, firmware_img_path, output_img_path):
    # Check if input files exist
    if not os.path.exists(rk2118_bin_path):
        print(f"Error: File '{rk2118_bin_path}' not found")
        sys.exit(1)

    if not os.path.exists(firmware_img_path):
        print(f"Error: File '{firmware_img_path}' not found")
        sys.exit(1)

    # Read binary data
    with open(rk2118_bin_path, 'rb') as f:
        rk2118_data = f.read()

    with open(firmware_img_path, 'rb') as f:
        firmware_data = f.read()

    # Ensure rk2118_data is 512-byte aligned
    if len(rk2118_data) % 512 != 0:
        rk2118_data += b'\x00' * (512 - len(rk2118_data) % 512)

    # Calculate jhash
    combined_data = rk2118_data + firmware_data
    jhash = JS_Hash_c(combined_data) if use_js_hash else 0

    # Header information
    header = bytearray(512)
    struct.pack_into('<4sII4x32sII8x32sII8x', header, 0, b'RKFP', jhash, use_js_hash,
                     b'rk2118_upgrade.bin', 512, len(rk2118_data),
                     b'Firmware.img', 512 + len(rk2118_data), len(firmware_data))

    # Write to output file
    with open(output_img_path, 'wb') as f:
        f.write(header)
        f.write(rk2118_data)
        f.write(firmware_data)

    print(f"Successfully created {output_img_path}")

# File paths
output_img_path = 'Image/rk2118_spi2apb_update.img'

use_js_hash = True

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: package_spi2apb_image.py <bin file> <img file>")
        print("       package_spi2apb_image.py rkbin/rk2118_upgrade_auto.bin Image/Firmware.img")
        sys.exit(1)

    rk2118_bin_path = sys.argv[1]
    firmware_img_path = sys.argv[2]

    # Create the firmware update
    create_firmware_update(rk2118_bin_path, firmware_img_path, output_img_path)

