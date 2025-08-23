#! /bin/bash

CUR_DIR=$(pwd)

usage() {
    echo "usage: ./build.sh <cpu_id 0~2 or all>"
}

# TODO: Please plan the memory according to the actual needs of the project.
# Requiring 1M alignment.
CPU0_MEM_BASE=0x03900000
CPU1_MEM_BASE=0x03a00000
CPU2_MEM_BASE=0x03e00000
CPU0_MEM_SIZE=0x00100000
CPU1_MEM_SIZE=0x00100000
CPU2_MEM_SIZE=0x00100000

make_hal() {
    export FIRMWARE_CPU_BASE=$(eval echo \$CPU$1_MEM_BASE)
    export DRAM_SIZE=$(eval echo \$CPU$1_MEM_SIZE)
    export SHMEM_BASE=0x03b00000
    export SHMEM_SIZE=0x00100000
    export DSMC_SLAVE_MEM_BASE=0x06000000
    export DSMC_SLAVE_MEM_SIZE=0x02000000
    export CUR_CPU=$1
    make clean
    rm $CUR_DIR/hal$1.elf $CUR_DIR/hal$1.bin
    make -j8
    cp $CUR_DIR/TestDemo.elf $CUR_DIR/hal$1.elf
    mv $CUR_DIR/TestDemo.bin $CUR_DIR/hal$1.bin
}

case $1 in
    0|1|2) make_hal $1 ;;
    all) make_hal 0; make_hal 1; make_hal 2;;
    *) usage; exit ;;
esac

cd ..
./mkimage.sh
cd -
