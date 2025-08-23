#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh"
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools
IMAGE=$(pwd)/Image

rm -rf $IMAGE/rtthread.bin $IMAGE/rtthread.img $IMAGE/Firmware*

cp -r $CUR_DIR/rtthread.bin $IMAGE/rtthread.img

$TOOLS/resource_header_tool pack --json $IMAGE/config.json $IMAGE/rtthread.img > /dev/null

echo 'Image: rtthread image is ready'
$TOOLS/firmware_merger -p $IMAGE/setting.ini $IMAGE/
