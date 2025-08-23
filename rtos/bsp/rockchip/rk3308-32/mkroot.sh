#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
    echo "usage: ./mkroot.sh <userdata_path> <parameter_file> <output_path>"
    echo "examp: ./mkroot.sh userdata/ parameter.txt Image/"
}

if [ "$1" == "" ] || [ "$2" == "" ] || [ "$3" == "" ]
then
    usage
    exit
fi

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools

RESOURCE_PATH="$1"
PARAM_FILE="$2"
OUTPUT_PATH="$3"
ROOT_NAME=root.img

#Del file whether exist
if [ ! -f "$OUTPUT_PATH$ROOT_NAME" ];then
    echo "root File not exist"
else
    rm -rf $OUTPUT_PATH$ROOT_NAME
fi

ROOT_PART_SIZE=`grep -r "CMDLINE" $PARAM_FILE | grep -wi "rootfs" | cut -d ',' -f 4 | cut -d '@' -f 1 | cut -d 'x' -f 2 `
echo "get partation size   is 0x$ROOT_PART_SIZE"

#transform to upper case
TRANS=`echo $ROOT_PART_SIZE | tr 'a-z' 'A-Z'`

#transform to 512B one sector unit
ROOT_EACH_SIZE=`echo "ibase=16; $TRANS" | tr -d $'\r' | bc`
#echo "userdata part size is :$ROOT_EACH_SIZE sector"

#transform to KB unit
ROOT_PART_SIZE=`echo "ibase=10; $ROOT_EACH_SIZE / 2" | bc`
#echo "userdata part size is :$ROOT_PART_SIZE KB"

#transform to 4096B(4KB) one sector unit
ROOT_SECTOR_SIZE=`echo "ibase=10;$ROOT_PART_SIZE / 4" | bc`
#echo "userdata part size is :$ROOT_SECTOR_SIZE *4KB"

dd of=$ROOT_NAME bs=4K seek=$ROOT_SECTOR_SIZE count=0 2>&1 || fatal "Failed to dd image!"
#dd if=/dev/zero of=$ROOT_NAME bs=1M count=1
if [ $? != 0 ];then
    rm -rf $ROOT_NAME
    echo "Error1: making $ROOT_NAME error."
    exit
fi

ROOTFS_SECTOR_SIZE=$(grep -r "RT_DFS_ELM_MAX_SECTOR_SIZE" rtconfig.h | cut -d ' ' -f 3)
if [ -z $ROOTFS_SECTOR_SIZE ];then
    ROOTFS_SECTOR_SIZE=512
fi
echo "rootfs sector size: $ROOTFS_SECTOR_SIZE"

mkfs.fat -S $ROOTFS_SECTOR_SIZE $ROOT_NAME
if [ $? != 0 ];then
    rm -rf $ROOT_NAME
    echo "Error2: making $ROOT_NAME error."
    exit
fi

MTOOLS_SKIP_CHECK=1 $TOOLS/mcopy -bspmn -D s -i $ROOT_NAME $RESOURCE_PATH* ::/
if [ $? != 0 ];then
    rm -rf $ROOT_NAME
    echo "Error3: making $ROOT_NAME error."
else
    mv $ROOT_NAME $OUTPUT_PATH
    echo "Making $ROOT_NAME with size = $ROOT_EACH_SIZE*512B = $ROOT_PART_SIZE*1KB = $ROOT_SECTOR_SIZE*4KB sucessful."
fi
