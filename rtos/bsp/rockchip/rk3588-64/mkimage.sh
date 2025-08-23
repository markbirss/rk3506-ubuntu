#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh [its]"
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools
IMAGE=$(pwd)/Image

if [ ! -n "$1" ] ;then
    $TOOLS/mkimage -f $IMAGE/smp.its -E -p 0x1c00 $IMAGE/amp.img
else
    $TOOLS/mkimage -f $1 -E -p 0x1c00 $IMAGE/amp.img
fi

echo 'Image: amp.img is ready.'
