#!/usr/bin/env python3

import sys

def convert_partition_info(input_file, start_offset):
    part_offset = start_offset
    output = ""

    with open(input_file, "r") as file:
        for line in file:
            # 去掉行末的换行符和多余的空白
            line = line.strip()

            # 跳过空行
            if not line:
                continue

            # 分离分区名和大小
            part_name, part_size = line.split('=')
            part_name = part_name.strip()
            part_size = part_size.strip()

            if part_size == "grow":
                # 特殊处理grow情况
                output += f"-@0x{part_offset:08x}({part_name}:grow),"
            else:
                # 处理普通的十六进制大小
                part_size_value = int(part_size, 16)
                output += f"0x{part_size_value:08x}@0x{part_offset:08x}({part_name}),"
                part_offset += part_size_value

    # 去掉最后一个逗号
    output = output.rstrip(',')

    return output

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage   : ./mkparameter.py <input_file> <start_offset>")
        print("spi nor : ./mkparameter.py board/evb/part.txt 0x00000080")
        print("spi nand: ./mkparameter.py board/evb_nand/part.txt 0x00000000")
        print("emmc    : ./mkparameter.py board/evb_emmc/part.txt 0x00000000")
        sys.exit(1)

    input_file = sys.argv[1]
    start_offset = int(sys.argv[2], 16)  # 将传入的offset转换成16进制数值
    result = convert_partition_info(input_file, start_offset)
    print(result)
