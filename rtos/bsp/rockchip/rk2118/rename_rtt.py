#!/usr/bin/env python3

import os
import shutil

def contains_define_string(file_path, string_to_search):
    try:
        with open(file_path, 'r') as file:
            for line in file:
                if string_to_search in line:
                    return True
        return False
    except FileNotFoundError:
        print(f"The file {file_path} was not found.")
        return False
    except IOError:
        print(f"An IOError occurred when trying to read the file {file_path}.")
        return False

def copy_file(src, dst):
    try:
        shutil.copyfile(src, dst)
        print(f"File copy from {src} to {dst}")
    except FileNotFoundError:
        print(f"The file {src} was not found.")
    except OSError as e:
        print(f"Error renaming file: {e}")

def main():
    current_directory = os.getcwd()
    config_file = os.path.join(current_directory, 'rtconfig.h')
    bin_file = os.path.join(current_directory, 'rtthread.bin')
    elf_file = os.path.join(current_directory, 'rtthread.elf')

    if contains_define_string(config_file, '#define RK2118_CPU_CORE1'):
        copy_file(bin_file, os.path.join(current_directory, 'rtt1.bin'))
        copy_file(elf_file, os.path.join(current_directory, 'rtt1.elf'))
    elif contains_define_string(config_file, '#define RT_BUILD_RECOVERY_FW'):
        copy_file(bin_file, os.path.join(current_directory, 'recovery.bin'))
        copy_file(elf_file, os.path.join(current_directory, 'recovery.elf'))
    elif contains_define_string(config_file, '#define RT_BUILD_OSB'):
        copy_file(bin_file, os.path.join(current_directory, 'rtt0_b.bin'))
        copy_file(elf_file, os.path.join(current_directory, 'rtt0_b.elf'))
    else:
        copy_file(bin_file, os.path.join(current_directory, 'rtt0.bin'))
        copy_file(elf_file, os.path.join(current_directory, 'rtt0.elf'))

if __name__ == "__main__":
    main()
