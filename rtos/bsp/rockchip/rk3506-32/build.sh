#!/bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Image

usage() {
    echo "usage:"
    echo "    $0 <cpu_id 0~2 or all>"
    echo "    $0 root <resource_file> <PAGE_SIZE(bytes)>"
    echo "    $0 userdata <resource_file> <PAGE_SIZE_KB> <BLOCK_SIZE_KB> <PARTITION_SIZE_KB>"
    echo "like:"
    echo "    $0 0"
    echo "    $0 root resource/root 2048"
    echo "            page size 2048 bytes"
    echo "    $0 userdata resource/userdata 2 128 180224"
    echo "            page size 2KB  block size 12KB  partition size 180224KB"
}

# TODO: Please plan the memory according to the actual needs of the project.
# Requiring 1M alignment.
CPU0_MEM_BASE=0x03900000
CPU1_MEM_BASE=0x03a00000
CPU2_MEM_BASE=0x03e00000
CPU0_MEM_SIZE=0x00100000
CPU1_MEM_SIZE=0x00100000
CPU2_MEM_SIZE=0x00100000

make_rtt() {
    export RTT_PRMEM_BASE=$(eval echo \$CPU$1_MEM_BASE)
    export RTT_PRMEM_SIZE=$(eval echo \$CPU$1_MEM_SIZE)
    export RTT_SHMEM_BASE=0x03b00000
    export RTT_SHMEM_SIZE=0x00100000
    export LINUX_RPMSG_BASE=0x03c00000
    export LINUX_RPMSG_SIZE=0x00200000
    export CUR_CPU=$1
    scons -c
    rm -rf $CUR_DIR/gcc_arm.ld $IMAGE/amp.img $IMAGE/rtt$1.elf $IMAGE/rtt$1.bin
    scons -j32
    cp $CUR_DIR/rtthread.elf $IMAGE/rtt$1.elf
    mv $CUR_DIR/rtthread.bin $IMAGE/rtt$1.bin
}

case $1 in
    0|1|2)
        make_rtt $1
        ;;
    root)
        resource_file=$2
        parameter_file="board/evb1/parameter-128M-smp.txt"
        PAGE_SIZE=$3
        ./mkroot.sh root $resource_file $parameter_file $PAGE_SIZE || { echo "Error: mkroot.sh failed"; exit 1; }
        exit 0
        ;;
    userdata)
        resource_file=$2
        target_file="Image/userdata.img"
        PAGE_SIZE_KB=$3
        BLOCK_SIZE_KB=$4
        PARTITION_SIZE_KB=$5
        ./mkroot.sh nand $resource_file $target_file $PAGE_SIZE_KB $BLOCK_SIZE_KB $PARTITION_SIZE_KB || { echo "Error: mkroot.sh failed"; exit 1; }
        exit 0
        ;;
    all)
        for i in {0..2};
            do make_rtt $i;
        done
        ;;
    *)
        usage; exit ;;
esac

if grep -q "#define RT_USING_SMP" rtconfig.h; then
    ./mkimage.sh smp
else
    ./mkimage.sh amp
fi
