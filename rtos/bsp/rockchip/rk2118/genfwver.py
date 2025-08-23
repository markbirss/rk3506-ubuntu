#!/usr/bin/env python3

import re
import struct
import sys

def extract_firmware_version(parameter_file):
    with open(parameter_file, 'r') as file:
        for line in file:
            match = re.search(r'FIRMWARE_VER:\s*([\d\.]+)', line)
            if match:
                version_str = match.group(1)
                major, minor, small = map(int, version_str.split('.'))
                firmware_version = (major << 24) | (minor << 16) | (small)
                return firmware_version
    return None

def replace_firmware_version(c_file, firmware_version):
    with open(c_file, 'r') as file:
        content = file.read()

    hex_version = f"0x{firmware_version:08X}"
    content = re.sub(r'uint32_t firmware_version = 0x[0-9A-Fa-f]+;',
                     f'uint32_t firmware_version = {hex_version};',
                     content)

    with open(c_file, 'w') as file:
        file.write(content)

    print(f"Replaced firmware_version with {hex_version}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: ./genfwver.py <parameter_file> <c_file>")
        print("       ./genfwver.py board/evb_nand/parameter.txt ../common/fwmgr/fw_analysis.c")
        sys.exit(1)

    parameter_file = sys.argv[1]
    c_file = sys.argv[2]

    firmware_version = extract_firmware_version(parameter_file)
    if firmware_version is not None:
        replace_firmware_version(c_file, firmware_version)
    else:
        print("FIRMWARE_VER not found in parameter file.")

