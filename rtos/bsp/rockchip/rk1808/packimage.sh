#! /bin/bash

usage() {
	echo "usage: ./packimage.sh"
}

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Bin

rm -rf $IMAGE/rtthread.bin $IMAGE/rtthread.img $IMAGE/Firmware*

cp -r $CUR_DIR/rtthread.bin $IMAGE/

cd ${IMAGE} && ./firmware_merger -P 1808spi_linux.ini ./

echo 'Image: rtthread image is ready'
