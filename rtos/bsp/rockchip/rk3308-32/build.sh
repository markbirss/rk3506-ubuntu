#!/bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Image
PARAM_FILE=$CUR_DIR/Image/parameter.txt
TOOLS=$CUR_DIR/../tools

SMP=$(grep -wE "RT_USING_SMP" $CUR_DIR/rtconfig.h)
if [ -n "$SMP" ];then
    ITS_FILE=$CUR_DIR/Image/smp.its
else
    ITS_FILE=$CUR_DIR/Image/amp.its
fi

usage() {
    echo "usage:"
    echo "./build.sh <cpu_id 0~3>    make CPU0~CPU3"
    echo "./build.sh <rootfs>        make rootfs"
    echo "./build.sh <image>         make image"
    echo "./build.sh <all>           build all"
    echo "./build.sh <clean>         clean all"
    echo ""
}

SHRPMSG_SIZE=0x00080000
SHRAMFS_SIZE=0x00020000
SHLOG0_SIZE=0x00001000
SHLOG1_SIZE=0x00001000
SHLOG2_SIZE=0x00001000
SHLOG3_SIZE=0x00001000

ROOT_PART_OFFSET=
ROOT_PART_SIZE=
check_rootfs_partition()
{
    local i

    for ((i = 1; i < 99; i++)); do
        PARTITION_FIELD=$(grep -r "CMDLINE" $PARAM_FILE | cut -d ',' -f ${i})
        if [ -z "$PARTITION_FIELD" ]; then
            break
        fi

        if [[ "$PARTITION_FIELD" == *"rootfs"* ]]; then
            ROOT_PART_OFFSET=$(grep -r "CMDLINE" $PARAM_FILE | cut -d ',' -f ${i} | cut -d '(' -f 1 | cut -d '@' -f 2)
            ROOT_PART_SIZE=$(grep -r "CMDLINE" $PARAM_FILE | cut -d ',' -f ${i} | cut -d '(' -f 1  | cut -d '@' -f 1)
            break
        fi
    done
}

make_rtt()
{
    echo "=========================================="
    echo "  Building RT-Thread CPU$1 Start"
    echo "=========================================="

    export RTT_PRMEM_BASE=$(grep -wE "amp[0-9]* {|load" $ITS_FILE | grep amp$1 -A1 | grep load | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_PRMEM_BASE=$RTT_PRMEM_BASE"
    export RTT_PRMEM_SIZE=$(grep -wE "amp[0-9]* {|size" $ITS_FILE | grep amp$1 -A1 | grep size | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_PRMEM_SIZE=$RTT_PRMEM_SIZE"
    export RTT_SRAM_BASE=$(grep -wE "amp[0-9]* {|srambase" $ITS_FILE | grep amp$1 -A1 | grep srambase | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_SRAM_BASE=$RTT_SRAM_BASE"
    export RTT_SRAM_SIZE=$(grep -wE "amp[0-9]* {|sramsize" $ITS_FILE | grep amp$1 -A1 | grep sramsize | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_SRAM_SIZE=$RTT_SRAM_SIZE"
    export RTT_SHMEM_BASE=$(grep -wE -A3 "share_memory {" $ITS_FILE | grep base | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_SHMEM_BASE=$RTT_SHMEM_BASE"
    export RTT_SHMEM_SIZE=$(grep -wE -A3 "share_memory {" $ITS_FILE | grep size | cut -d'<' -f2 | cut -d'>' -f1)
    echo "RTT_SHMEM_SIZE=$RTT_SHMEM_SIZE"

    check_rootfs_partition
    if [ -n "$ROOT_PART_OFFSET" ] && [ -n "$ROOT_PART_SIZE" ];then
        export ROOT_PART_OFFSET=$ROOT_PART_OFFSET
        echo "ROOT_PART_OFFSET=$ROOT_PART_OFFSET"
        export ROOT_PART_SIZE=$ROOT_PART_SIZE
        echo "ROOT_PART_SIZE=$ROOT_PART_SIZE"
    fi

    export RTT_SHRPMSG_SIZE=$SHRPMSG_SIZE
    export RTT_SHRAMFS_SIZE=$SHRAMFS_SIZE
    export RTT_SHLOG0_SIZE=$SHLOG0_SIZE
    export RTT_SHLOG1_SIZE=$SHLOG1_SIZE
    export RTT_SHLOG2_SIZE=$SHLOG2_SIZE
    export RTT_SHLOG3_SIZE=$SHLOG3_SIZE
    export CUR_CPU=$1

    scons -c > /dev/null
    rm -rf $CUR_DIR/gcc_arm.ld $IMAGE/amp.img $IMAGE/rtt$1.elf $IMAGE/rtt$1.bin $IMAGE/rtt$1.log
    scons -j8 > $IMAGE/rtt$1.log 2>&1
    cp $CUR_DIR/rtthread.elf $IMAGE/rtt$1.elf
    mv $CUR_DIR/rtthread.bin $IMAGE/rtt$1.bin

    echo "Build RT-Thread CPU$1 Successful!"
}

CORE_NUMBERS=$(grep -wcE "amp[0-9]* {" $ITS_FILE)

case $1 in
    0|1|2|3)
		if ! [ "$1" -lt "$CORE_NUMBERS" ] 2>/dev/null; then
			exit 1
		fi
        make_rtt $1
        ;;
    rootfs)
        $CUR_DIR/mkroot.sh $CUR_DIR/userdata $PARAM_FILE $IMAGE/
        ;;
    image)
        $TOOLS/mkimage -f $ITS_FILE -E -p 0xe00 $IMAGE/amp.img
        ;;
    all)
        for ((i = 0; i < $CORE_NUMBERS; i++)); do
            make_rtt $i
        done

        $CUR_DIR/mkroot.sh $CUR_DIR/userdata $PARAM_FILE $IMAGE/
        $TOOLS/mkimage -f $ITS_FILE -E -p 0xe00 $IMAGE/amp.img
        ;;
    clean)
        scons -c > /dev/null
        rm -f $IMAGE/rtt*.elf $IMAGE/rtt*.bin $IMAGE/root.img $IMAGE/amp.img  $IMAGE/rtt*.log
        rm -f gcc_arm.ld
        ;;
    *)
        usage;
        exit
        ;;
esac
