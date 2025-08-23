#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh [partition_setting]"
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools
IMAGE=$(pwd)/Image

rm -rf $CUR_DIR/rtthread.bin $IMAGE/rtthread.img $CUR_DIR/rtthread_bak.bin $IMAGE/rtthread_bak.img $IMAGE/Firmware*

arm-none-eabi-objcopy -O binary rtthread.elf rtthread.bin
cp -r $CUR_DIR/rtthread.bin $IMAGE/rtthread.img
$TOOLS/resource_header_tool pack --json $IMAGE/config.json $IMAGE/rtthread.img > /dev/null

if [ -f "rtthread_bak.elf" ]; then
    arm-none-eabi-objcopy -O binary rtthread_bak.elf rtthread_bak.bin
    cp -r $CUR_DIR/rtthread_bak.bin $IMAGE/rtthread_bak.img
    $TOOLS/resource_header_tool pack --json $IMAGE/config.json $IMAGE/rtthread_bak.img > /dev/null
fi

echo 'Image: rtthread image is ready'

if [ ! -n "$1" ] ;then
    $TOOLS/firmware_merger -p board/common/setting.ini $IMAGE/
else
    $TOOLS/firmware_merger -p $1 $IMAGE/
fi
