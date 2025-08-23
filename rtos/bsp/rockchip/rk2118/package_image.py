#!/usr/bin/env python3

import sys
import os
import subprocess
import json
import re

xip = True

def get_vectors_section_address(elf_file):
    try:
        # 使用readelf命令获取ELF文件的section信息
        output = subprocess.check_output(['readelf', '-S', elf_file], universal_newlines=True)

        # 解析readelf命令的输出
        for line in output.split('\n'):
            if '.vectors' in line:
                # 提取.vectors section的起始地址
                address = line.split()[4]
                return int(address, 16)

        return None
    except FileNotFoundError:
        print(f"{elf_file} not found")
        return None
    except subprocess.CalledProcessError as e:
        print(f"readelf error: {e}")
        return None

def find_part_offset(file_path, search_string):
    with open(file_path, 'r') as file:
        lines = file.readlines()

        for i in range(len(lines)):
            if search_string in lines[i]:
                offset_line = lines[i-3]
                if offset_line.startswith('PartOffset='):
                    offset_value = offset_line.split('=')[1].strip()
                    return offset_value

    return None

def find_part_offset_parameter(file_path, search_string):
    pattern = re.compile(r'(0x[0-9a-fA-F]+)@(0x[0-9a-fA-F]+)\s*\({}\)'.format(re.escape(search_string)))

    offset = None
    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                offset = match.group(2).strip()
                #print(f"{file_path} {search_string} part offset: {offset}")
                break

    return offset

def replace_loading_address(file_path, new_address, xip_flag):
    try:
        with open(file_path, 'r') as file:
            data = json.load(file)

        if xip_flag:
            new_address -= 0x10000000
        else:
            new_address -= 0x40000000
        data['LOADING_ADDRESS'] = f"0x{int(new_address):08X}"

        with open(file_path, 'w') as file:
            json.dump(data, file, indent=4)
            file.write('\n')

        print(f"Replaced LOADING_ADDRESS with 0x{int(new_address):08X} in {file_path}")

    except FileNotFoundError:
        print(f"File {file_path} not found.")
    except json.JSONDecodeError:
        print(f"Error decoding JSON from file {file_path}.")
    except Exception as e:
        print(f"An error occurred: {e}")

def check_xip_addr(setting_file):
    if "setting.ini" in setting_file:
        search_list = ['File=../../Image/rtt0.img', 'File=../../Image/rtt0_b.img', 'File=../../Image/rtt1.img', 'File=../../Image/recovery.img']
    else:
        parameter_file = setting_file.replace("package-file", "parameter.txt")
        search_list = ['cpu0_os_a', 'cpu0_os_b', 'cpu1_os_a', 'recovery']
        img_list = ['rtt0.img', 'rtt0_b.img', 'rtt1.img', 'recovery.img']

    for index, search_string in enumerate(search_list):
        # 查找并打印PartOffset，如果没找到就找下一个
        if "setting.ini" in setting_file:
            part_offset = find_part_offset(setting_file, search_string)
        else:
            part_offset = find_part_offset_parameter(parameter_file, search_string)
        if part_offset is None :
            print(f"not found PartOffset for {search_string}, skipping")
            continue

        part_offset = 512 * int(part_offset, 16)
        part_offset += 0x11000000 + 0x800
        print(f"found {search_string} PartOffset: 0x{part_offset:x}")

        # 获取.vectors section的起始地址，如果不在XIP范围内，则忽略
        if "setting.ini" in setting_file:
            elf_file = search_string.split('/')[3].strip().replace('.img', '.elf')
            json_file = search_string.split('/')[3].strip().replace('.img', '.json')
        else:
            elf_file = img_list[index].replace('.img', '.elf')
            json_file = img_list[index].replace('.img', '.json')

        if os.path.exists(elf_file) :
            vectors_address = get_vectors_section_address(elf_file)
            if vectors_address is None:
                print(f"can not found .vectors section in {elf_file}")
                continue

            if elf_file == "recovery.elf":
                vectors_address += 0x10000000

            if vectors_address < 0x11000000 or vectors_address >= 0x20000000 :
                global xip
                xip = False
                print("vectors address is not in the XIP range, check ignore")
                if elf_file == "rtt0.elf" or elf_file == "rtt1.elf" :
                    replace_loading_address(f"Image/config/xip_off/{json_file}", vectors_address, False)
                continue

            if part_offset == vectors_address :
                print(f"{elf_file} part_offset==vectors_address")
            else:
                print(f"Error: {elf_file} part_offset=0x{part_offset:x}, xip_address=0x{vectors_address:x}")

            replace_loading_address(f"Image/config/xip_on/{json_file}", vectors_address, True)

def adjust_path(path):
    parts = path.split('/')
    if parts[0] == '..' and parts[1] == '..':
        new_path = '/'.join(parts[2:])
        return f"{new_path}"
    return path

def extract_dsp_paths(package_file):
    dsp0_dir = None
    dsp1_dir = None
    dsp2_dir = None

    with open(package_file, 'r') as file:
        for line in file:
            if 'dsp0.bin' in line and dsp0_dir is None:
                dsp0_dir = adjust_path(line.split()[1].strip())
            elif 'dsp1.bin' in line and dsp1_dir is None:
                dsp1_dir = adjust_path(line.split()[1].strip())
            elif 'dsp2.bin' in line and dsp2_dir is None:
                dsp2_dir = adjust_path(line.split()[1].strip())

            if dsp0_dir and dsp1_dir and dsp2_dir:
                break

    return dsp0_dir, dsp1_dir, dsp2_dir

def package_dsp_fw(setting_file):
    base_path = "/".join(setting_file.split("/")[:-1])
    package_file = f"{base_path}/package-file"

    dsp0_dir, dsp1_dir, dsp2_dir = extract_dsp_paths(package_file)

    if os.path.exists("./Image/dsp0.img"):
        subprocess.call((f"rm ./Image/dsp0.img"), shell=True)
    if os.path.exists("./Image/dsp1.img"):
        subprocess.call((f"rm ./Image/dsp1.img"), shell=True)
    if os.path.exists("./Image/dsp2.img"):
        subprocess.call((f"rm ./Image/dsp2.img"), shell=True)

    if os.path.exists(dsp0_dir):
        print(f"cp {dsp0_dir} ./Image/dsp0.img")
        subprocess.call((f"cp {dsp0_dir} ./Image/dsp0.img"), shell=True)
        subprocess.call((f"../tools/resource_header_tool pack --json Image/config/dsp.json ./Image/dsp0.img"), shell=True)
    else:
        print(f"Error: cat found {dsp0_dir}")

    if os.path.exists(dsp1_dir):
        print(f"cp {dsp1_dir} ./Image/dsp0.img")
        subprocess.call((f"cp {dsp1_dir} ./Image/dsp1.img"), shell=True)
        subprocess.call((f"../tools/resource_header_tool pack --json Image/config/dsp.json ./Image/dsp1.img"), shell=True)
    else:
        print(f"Error: cat found {dsp1_dir}")

    if os.path.exists(dsp2_dir):
        print(f"cp {dsp2_dir} ./Image/dsp0.img")
        subprocess.call((f"cp {dsp2_dir} ./Image/dsp2.img"), shell=True)
        subprocess.call((f"../tools/resource_header_tool pack --json Image/config/dsp.json ./Image/dsp2.img"), shell=True)
    else:
        print(f"Error: cat found {dsp2_dir}")

def package_fw(setting_file):
    if xip :
        subprocess.call((f"cp ./rkbin/tfm_s.bin ./Image/tfm_s.img"), shell=True)
        subprocess.call((f"./align_bin_size.sh ./Image/tfm_s.img"), shell=True)
        subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_on/tfm_s.json ./Image/tfm_s.img"), shell=True)
        if os.path.exists("./rkbin/tfm_s_b.bin"):
            subprocess.call((f"cp ./rkbin/tfm_s_b.bin ./Image/tfm_s_b.img"), shell=True)
            subprocess.call((f"./align_bin_size.sh ./Image/tfm_s_b.img"), shell=True)
            subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_on/tfm_s_b.json ./Image/tfm_s_b.img"), shell=True)
        if os.path.exists("./rkbin/tfm_s_mini.bin"):
            subprocess.call((f"cp ./rkbin/tfm_s_mini.bin ./Image/tfm_s_mini.img"), shell=True)
            subprocess.call((f"./align_bin_size.sh ./Image/tfm_s_mini.img"), shell=True)
            subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_on/tfm_s.json ./Image/tfm_s_mini.img"), shell=True)
    else :
        subprocess.call((f"cp ./rkbin/tfm_s_ddr.bin ./Image/tfm_s.img"), shell=True)
        subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_off/tfm_s.json ./Image/tfm_s.img"), shell=True)

    if xip :
        subprocess.call((f"cp ./rkbin/cpu1_loader.bin ./Image/cpu1_loader.img"), shell=True)
        subprocess.call((f"./align_bin_size.sh ./Image/cpu1_loader.img"), shell=True)
        subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_on/cpu1_loader.json ./Image/cpu1_loader.img"), shell=True)
    else :
        if os.path.exists("./rkbin/cpu1_loader_ddr.bin"):
            subprocess.call((f"cp ./rkbin/cpu1_loader_ddr.bin ./Image/cpu1_loader.img"), shell=True)
            subprocess.call((f"./align_bin_size.sh ./Image/cpu1_loader.img"), shell=True)
            subprocess.call(("../tools/resource_header_tool pack --json Image/config/xip_off/cpu1_loader.json ./Image/cpu1_loader.img"), shell=True)

    search_list = ['../../Image/rtt0.img', '../../Image/rtt0_b.img', '../../Image/rtt1.img', '../../Image/recovery.img']
    for search_string in search_list:

        img_file = search_string.split('/')[3].strip()
        bin_file = search_string.split('/')[3].strip().replace('.img', '.bin')
        json_file = search_string.split('/')[3].strip().replace('.img', '.json')

        if not os.path.exists(bin_file):
            print(f"File {bin_file} does not exist, skipping.")
            continue

        subprocess.call((f"cp {bin_file} ./Image/{img_file}"), shell=True)
        if xip :
            print("package xip image")
            subprocess.call((f"../tools/resource_header_tool pack --json Image/config/xip_on/{json_file} ./Image/{img_file}"), shell=True)
        else :
            print("package normal image")
            subprocess.call((f"../tools/resource_header_tool pack --json Image/config/xip_off/{json_file} ./Image/{img_file}"), shell=True)

    if xip == False :
        package_dsp_fw(setting_file)

def update_package_file(setting_file):
    base_path = "/".join(setting_file.split("/")[:-1])
    package_file = f"{base_path}/package-file"

    with open(package_file, 'r') as file:
        lines = file.readlines()

    replacements = {
        "dsp0.bin": "../../Image/dsp0.img",
        "dsp1.bin": "../../Image/dsp1.img",
        "dsp2.bin": "../../Image/dsp2.img"
    }

    with open(package_file, 'w') as file:
        for line in lines:
            for dsp_bin, new_path in replacements.items():
                if dsp_bin in line:
                    line = f"{line.split()[0]}\t{new_path}\n"
            file.write(line)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: " + sys.argv[0] + " <setting.ini or package-file>")
        sys.exit(1)

    setting_file = sys.argv[1]
    if os.path.exists(setting_file) :
        if setting_file.endswith('.ini') :
            check_xip_addr(setting_file)
            package_fw(setting_file)
        else :
            check_xip_addr(setting_file)
            package_fw(setting_file)
            update_package_file(setting_file)
    print("package image finish")
    sys.exit(0)
