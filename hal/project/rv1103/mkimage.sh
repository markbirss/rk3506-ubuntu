#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh"
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../../tools
IMAGE=$(pwd)/Image

$TOOLS/mkimage -f $IMAGE/hal.its -E -p 0x800 $IMAGE/hal.img

echo 'Image: hal.img is ready.'
