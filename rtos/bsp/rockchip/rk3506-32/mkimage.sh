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
    $TOOLS/mkimage -f $IMAGE/amp_linux.its -E -p 0xe00 $IMAGE/amp.img
else
    case $1 in
        *"smp"*)
            $TOOLS/mkimage -f $IMAGE/smp.its -E -p 0xe00 $IMAGE/amp.img
            ;;
        *"amp"*)
            $TOOLS/mkimage -f $IMAGE/amp_linux.its -E -p 0xe00 $IMAGE/amp.img
            ;;
        *)
            usage
            exit
            ;;
    esac
fi

echo 'Image: amp.img is ready.'
