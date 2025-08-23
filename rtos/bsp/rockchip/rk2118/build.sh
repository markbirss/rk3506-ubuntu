#!/bin/bash

usage () {
    echo "Usage:"
    echo "    $0 [partition_setting] [rtt0|rtt1|recovery|soundbar|partybox|gamebar]"
    echo ""
    echo "    Build all firmware"
    echo "        $0 board/adsp_demo/setting.ini      : build for board RK_AUTOMOTIVE_DSP_DEMO_RK2118M_V10_2024-01-25 "
    echo "                                                           or RK_EVB2_AUTOMOTIVE_DSP_RK2118M_V20_20240415, will generate Firmware.img"
    echo "        $0 board/evb/package-file           : build for board RK_EVB1_RK2118G_V10_20240219 with spi nor flash, will generate update.img"
    echo "        $0 board/evb_nand/package-file      : build for board RK_EVB1_RK2118G_V10_20240219 with spi nand flash, will generate update.img"
    echo "        $0 board/evb1_v20/package-file      : build for board RK_EVB1_RK2118G_V20_20240424 with spi nand flash, will generate update.img"
    echo "        $0 board/evb3_v10/package-file      : build for board RK_EVB3_RK2118G_V10_20240715 with spi nand flash, will generate update.img"
    echo ""
    echo "    Build single firmware"
    echo "        $0 board/evb/package-file rtt0      : build rtt0 for cpu0 with board evb"
    echo "        $0 board/evb/package-file rtt1      : build rtt1 for cpu1 with board evb"
    echo "        $0 board/evb/package-file recovery  : build recovery for cpu0 with board evb"
    echo ""
    echo "    Build soundbar demo"
    echo "        $0 board/evb/package-file soundbar  : use components/hifi4/rtt/dsp_fw/evb_soundbar_demo/"
    echo "                                                  components/hifi4/rtt/rkstudio_bin/evb_soundbar_demo.bin"
}

build_rtt () {
    if [ "$1" -eq 0 ]; then
        echo "$separator_srt start build rtt0 $separator_srt"
    elif [ "$1" -eq 1 ]; then
        echo "$separator_srt start build rtt1 $separator_srt"
    elif [ "$1" -eq 2 ]; then
        echo "$separator_srt start build recovery $separator_srt"
    elif [ "$1" -eq 3 ]; then
        echo "$separator_srt start build rtt0_b $separator_srt"
    else
        exit 1
    fi

    echo "$separator_srt scons -c $separator_srt"
    scons -c

    # Copy the configuration file
    echo "$separator_srt copy $2 $separator_srt"
    cp $2 .config
    if [ $? -ne 0 ]; then
        exit 1
    fi

    # scons --useconfig=.config
    echo "$separator_srt scons --useconfig=.config $separator_srt"
    scons --useconfig=.config

    # scons --menuconfig
    echo "$separator_srt scons --menuconfig $separator_srt"
    scons --menuconfig

    if [ "$1" -eq 3 ]; then
        sed -i '/#define RT_CONFIG_H__/a \\n#define RT_BUILD_OSB' "rtconfig.h"
    fi

    # Build the project
    echo "$separator_srt scons $separator_srt"
    scons -j$(nproc)
    if [ "$?" -gt 0 ]; then
        exit 1
    fi
}

# Check argument
if [ "$#" -lt 1 ]; then
    usage
    exit 1
fi

TOOLCHAIN_NAME="arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi"
if [[ "$RTT_EXEC_PATH" != *"$TOOLCHAIN_NAME"* ]]; then
    echo "You must configure the correct toolchain: $TOOLCHAIN_NAME"
    echo "export RTT_EXEC_PATH=/path/to/toolchain/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin"
    exit 1
fi

setting_file=$1

if [ ! -f "$setting_file" ]; then
    echo "Error: $setting_file not found."
    usage
    exit 1
fi

echo "config_file=$setting_file"

BOARD_NAME=$(echo "$setting_file" | awk -F'/' '{print $2}')
echo "BOARD_NAME=$BOARD_NAME"

DEFCONFIG_PATH0="board/${BOARD_NAME}/defconfig"
DEFCONFIG_PATH2="board/${BOARD_NAME}/defconfig_recovery"
DEFCONFIG_PATH1="board/${BOARD_NAME}/cpu1_defconfig"

#if grep -q "CONFIG_RT_USING_SNOR=y" "$DEFCONFIG_PATH0"; then
#    echo "RTT_BUILD_XIP=Y when CONFIG_RT_USING_SNOR=y"
if [[ $setting_file == *".ini"* ]]; then
    echo "RTT_BUILD_XIP=Y when using $setting_file"
    export RTT_BUILD_XIP=Y
else
    echo "RTT_BUILD_XIP=N"
    export RTT_BUILD_XIP=N
fi

separator_srt="--------------------------------"

if [[ $setting_file == *"package-file"* ]]; then
    parameter_file=$(echo $setting_file | awk -F/ '{print $1 "/" $2 "/" "parameter.txt"}')
    ./genfwver.py $parameter_file ../common/fwmgr/fw_analysis.c
fi

if [ "$#" -eq 2 ]; then
    if [[ "$2" == "rtt0" || "$2" == "rtt0_b" || "$2" == "rtt1" || "$2" == "recovery" ]]; then
        echo "$separator_srt build single firmware $2 $separator_srt"

        if [ "$2" == "rtt0" ]; then
            build_rtt 0 $DEFCONFIG_PATH0
        fi

        if [ "$2" == "rtt0_b" ]; then
            build_rtt 3 $DEFCONFIG_PATH0
        fi

        if [ "$2" == "rtt1" ]; then
            build_rtt 1 $DEFCONFIG_PATH1
        fi

        if [ "$2" == "recovery" ]; then
            build_rtt 2 $DEFCONFIG_PATH2
        fi

        exit 0
    elif [[ "$2" == "soundbar" || "$2" == "partybox" || "$2" == "gamebar" ]]; then
        echo ""
    else
        usage
        exit 1
    fi
fi

echo "$separator_srt build all firmware $separator_srt"

build_cpu0=1
build_cpu1=0
build_recovery=0
build_cpu0_b=0

if grep -q "rtt0.img" "$setting_file"; then
    build_cpu0=1
fi

if grep -q "rtt0_b.img" "$setting_file"; then
    build_cpu0_b=1
fi

if grep -q "rtt1.img" "$setting_file"; then
    build_cpu1=1
fi

if grep -q "recovery.img" "$setting_file"; then
    build_recovery=1
fi

if [ -f recovery.bin ]; then
    rm recovery.bin
    rm recovery.elf
fi

if [ -f rtt0.bin ]; then
    rm rtt0.bin
    rm rtt0.elf
fi

if [ -f rtt0_b.bin ]; then
    rm rtt0_b.bin
    rm rtt0_b.elf
fi

if [ -f rtt1.bin ]; then
    rm rtt1.bin
    rm rtt1.elf
fi

if [ "$build_cpu1" -eq 1 ]; then
    build_rtt 1 $DEFCONFIG_PATH1
fi

if [ "$build_recovery" -eq 1 ]; then
    build_rtt 2 $DEFCONFIG_PATH2
fi

if [ "$build_cpu0" -eq 1 ]; then
    build_rtt 0 $DEFCONFIG_PATH0
fi

if [ "$build_cpu0_b" -eq 1 ]; then
    build_rtt 3 $DEFCONFIG_PATH0
fi

resource_file="resource/userdata/normal/"
if [[ $setting_file == *"package-file"* ]]; then
    parameter_file=$(echo $setting_file | awk -F/ '{print $1 "/" $2 "/" "parameter.txt"}')
    echo "-------------------------------- ./mkroot.sh $resource_file $parameter_file --------------------------------"
    ./mkroot.sh $resource_file $parameter_file || { echo "Error: mkroot.sh failed"; exit 1; }
else
    echo "-------------------------------- ./mkroot.sh $resource_file $setting_file --------------------------------"
    ./mkroot.sh $resource_file $setting_file || { echo "Error: mkroot.sh failed"; exit 1; }
fi

# Generate the image
echo "-------------------------------- ./mkimage.sh $setting_file $2 --------------------------------"
./mkimage.sh $setting_file $2
