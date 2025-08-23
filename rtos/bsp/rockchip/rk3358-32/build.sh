#!/bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Image

usage() {
    echo "usage: ./build.sh <cpu_id 0~3 or all>"
}

CPU0_MEM_BASE=0x02600000
CPU1_MEM_BASE=0x00800000
CPU2_MEM_BASE=0x01200000
CPU3_MEM_BASE=0x01c00000
CPU0_MEM_SIZE=0x00900000
CPU1_MEM_SIZE=0x00a00000
CPU2_MEM_SIZE=0x00a00000
CPU3_MEM_SIZE=0x00a00000

make_rtt() {
    export RTT_PRMEM_BASE=$(eval echo \$CPU$1_MEM_BASE)
    export RTT_PRMEM_SIZE=$(eval echo \$CPU$1_MEM_SIZE)
    export RTT_SHMEM_BASE=0x02f00000
    export RTT_SHMEM_SIZE=0x00100000
    export CUR_CPU=$1
    scons -c
    rm -rf $CUR_DIR/gcc_arm.ld $IMAGE/amp.img $IMAGE/rtt$1.elf $IMAGE/rtt$1.bin
    scons -j8
    cp $CUR_DIR/rtthread.elf $IMAGE/rtt$1.elf
    mv $CUR_DIR/rtthread.bin $IMAGE/rtt$1.bin
}

case $1 in
    0|1|2|3) make_rtt $1 ;;
    all) make_rtt 0; make_rtt 1; make_rtt 2; make_rtt 3 ;;
    *) usage; exit ;;
esac

#./mkimage.sh
