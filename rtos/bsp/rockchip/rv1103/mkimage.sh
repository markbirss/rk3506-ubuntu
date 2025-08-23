#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh"
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools
IMAGE=$(pwd)/Image

cp rtthread.bin $IMAGE/rtthread.bin
$TOOLS/mkimage -f $IMAGE/rtt.its -E -p 0x800 $IMAGE/rtt.img

echo 'Image: rtt.img is ready.'
