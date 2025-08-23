#! /bin/bash

usage() {
	echo "usage: ./packimage.sh" 
}

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Bin

rm -rf $IMAGE/rtthread.bin $IMAGE/rtthread.img $IMAGE/Firmware*

cp -r $CUR_DIR/rtthread.bin $IMAGE/

$IMAGE/kernelimage --pack --kernel $IMAGE/rtthread.bin $IMAGE/rtthread.img 0x62000000 > /dev/null

echo 'Image: rtthread image is ready'
$IMAGE/firmwareMerger -p $IMAGE/setting.ini $IMAGE/
