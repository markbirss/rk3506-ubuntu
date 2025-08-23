#!/bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

CUR_DIR=$(pwd)
IMAGE_DIR=$CUR_DIR/ImageAll
ROOT_DIR=$CUR_DIR/../../../../
HAL_DIR=$ROOT_DIR/hal

# Specify which OS will running on CPUs
# CPU1 must as master core, assigned to run RT-Thread
# CPU2, CPU3, CPU0 as sub core, assigned to run HAL
RTTCPU=1
HALCPUA=2
HALCPUB=3
HALCPUC=0

if [ $# == 0 ]; then
    echo ""
    echo "Please select 'uart2' or 'uart4' as debug port"
    echo "Usage: ./buildAll.sh <uart2 or uart4>"
    echo ""
    exit -1
else
    if [ $1 == "uart2" ]; then
        INI_FILE=../rkbin/RKBOOT/RK3562MINIALL.ini
    elif [ $1 == "uart4" ]; then
        INI_FILE=../rkbin/RKBOOT/RK3562MINIALL_UART4.ini
    else
        echo ""
        echo "Please select 'uart2' or 'uart4' as debug port"
        echo "Usage: ./buildAll.sh <uart2 or uart4>"
        echo ""
        exit -1
    fi
fi

build_uboot() {
    cd $ROOT_DIR/u-boot
    ./make.sh rk3562-aarch32-amp  $INI_FILE
    cd -

    cp $ROOT_DIR/u-boot/rk3562_loader_*.bin $IMAGE_DIR/
    cp $ROOT_DIR/u-boot/uboot.img $IMAGE_DIR/
    cp $ROOT_DIR/u-boot/trust.img $IMAGE_DIR/
}

build_hal() {
    cd $HAL_DIR/project/rk3562/GCC
    ./build.sh $HALCPUA
    ./build.sh $HALCPUB
    ./build.sh $HALCPUC
    cd -

    cp $HAL_DIR/project/rk3562/GCC/hal$HALCPUA.bin $IMAGE_DIR/cpu$HALCPUA.bin
    cp $HAL_DIR/project/rk3562/GCC/hal$HALCPUB.bin $IMAGE_DIR/cpu$HALCPUB.bin
    cp $HAL_DIR/project/rk3562/GCC/hal$HALCPUC.bin $IMAGE_DIR/cpu$HALCPUC.bin
}

build_rtt() {
    ./build.sh $RTTCPU
    cp $CUR_DIR/Image/rtt$RTTCPU.bin $IMAGE_DIR/cpu$RTTCPU.bin
}

build_all() {
    rm $IMAGE_DIR/*.bin
    rm $IMAGE_DIR/*.img

    build_uboot;
    build_rtt;
    build_hal;
}

build_all;
./mkimageAll.sh;
