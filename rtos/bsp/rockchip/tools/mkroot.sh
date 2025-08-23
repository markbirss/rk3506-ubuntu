#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
    echo "For Firmware.img (only for spi nor flash):"
    echo "usage: $0 <userdata_path> [partition_setting]"
    echo "Like:  $0 resource/userdata/normal/ board/evb/setting.ini"
}

usage_snand() {
    echo ""
    echo "For update.img:"
    echo "Make root.img, read only"
    echo "Usage: $0 <resource dir> <parameter>"
    echo "Like:  $0 resource/userdata/normal/ board/evb/parameter.txt"
    echo ""
    echo "Make userdata.img, r/w"
    echo "Usage: $0 <resource dir> <parameter> <flash page size KB> <flash block size KB> <flash size MB> [userdata filesystem size MB]"
    echo ""
    echo "Like: flash page size 2KB, block size 128KB, flash size 128MB, determine the file system size based on the file sizes in the resource dir"
    echo "    $0 resource/userdata/normal/ board/evb/parameter.txt 2 128 128"
    echo ""
    echo "Like: flash page size 2KB, block size 128KB, flash size 128MB, force userdata filesystem to 32MB"
    echo "    $0 resource/userdata/normal/ board/evb/parameter.txt 2 128 128 32"
}

check_root_is_last() {
    root_line=$(awk '/Name=root/ {print NR; exit}' $1)

    if [ -z "$root_line" ]; then
        echo "No 'Name=root' found"
        return 1
    fi

    next_userpart=$(awk "NR > $root_line && /^\[UserPart/ {print NR; exit}" $1)

    if [ -z "$next_userpart" ]; then
        return 1
    else
        return 0
    fi
}

make_mnt_point() {
    declare -a subdirs=("data" "udisk" "sdcard")

    for subdir in "${subdirs[@]}"; do
        if [ ! -d "$1/$subdir" ]; then
            mkdir "$1/$subdir"
        fi
    done
}

copy_resource() {
    mount_point=fat

    echo "rm -rf $mount_point"
    rm -rf $mount_point || exit 1
    echo "mkdir $mount_point"
    mkdir $mount_point || exit 1

    echo "fusefat $1 -o rw+ $mount_point"
    fusefat $1 -o rw+ $mount_point || exit 1

    make_mnt_point $mount_point

    echo "cp -r $2/* $mount_point"
    cp -r $2/* $mount_point > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Error: resource files is greater than root partition size"
        echo "Please enlarge root partition size"
        fusermount -u $mount_point
        exit 1
    fi
    ls -l $mount_point

    fusermount -u $mount_point || exit 1
}

make_root_fs() {
    parameter_file=$2
    resource_dir=$1
    IMAGE=Image
    ROOTIMG=root.img
    FATIMG=fat.img
    page_size="${3:-4096}"
    target_file=$4

    if [ ! -f "$parameter_file" ]; then
        echo "Error: file not found: $parameter_file"
        exit 1
    fi

    if [ ! -d "$resource_dir" ]; then
        echo "Error: dir not found: $resource_dir"
        exit 1
    fi

    if grep -q "(vnvm)" "$parameter_file"; then
        page_size=2048
    fi

    if grep -q "0x00002000@0x00000000(loader)" "$parameter_file"; then
        page_size=512
    fi

    root_size_sec=$(grep "CMDLINE" $parameter_file | awk -F',' '{for (i=1; i<=NF; i++) if ($i ~ /0x[0-9a-fA-F]+@0x[0-9a-fA-F]+\(root\)/) print $i}' | awk -F'@' '{print $1}')
    if [ -z "$root_size_sec" ]; then
        root_size_sec=$(grep "CMDLINE" $parameter_file | awk -F',' '{for (i=1; i<=NF; i++) if ($i ~ /0x[0-9a-fA-F]+@0x[0-9a-fA-F]+\(root_a\)/) print $i}' | awk -F'@' '{print $1}')
    fi

    if [ -n "$root_size_sec" ]; then
        decimal_value=$(($root_size_sec))
        root_size_byte=$((decimal_value * 512))
        echo "root_size_sec=0x$root_size_sec, root_size_byte=$root_size_byte"
    else
        echo "Error: root part not found in $parameter_file"
        exit 1
    fi

    if [ "$root_size_byte" -le $((2 * 1024 * 1024)) ]; then
        fat_size_4k=$(( root_size_byte / 4096 ))

        echo "dd if=/dev/zero of=$FATIMG bs=4K count=$fat_size_4k"
        dd if=/dev/zero of=$FATIMG bs=4K count=$fat_size_4k
    else
        # 计算 resource/* 文件的总大小
        total_size=$(du -c $resource_dir/* | grep total$ | awk '{print $1}')

        # 转换成MB并增加一定的余量，比如增加10%作为文件系统的元数据和空闲空间
        fat_size_mb=$(( (total_size / 1024) + 2 ))
        fat_size_mb=$(( fat_size_mb + (fat_size_mb / 10) ))

        fat_size_byte=$(( fat_size_mb * 1024 * 1024  ))

        echo "resource total_szie_kb=$total_size, resource fat_size_byte=$fat_size_byte, fat_size_mb=$fat_size_mb"

        if [ "$fat_size_byte" -gt "$root_size_byte" ]; then
            echo "Error: resource files is $fat_size_byte greater than root partition size $root_size_byte"
            echo "Please enlarge root partition size"
            exit 1
        fi

        # 创建相应大小的镜像文件
        echo "dd if=/dev/zero of=$FATIMG bs=1M count=$fat_size_mb"
        dd if=/dev/zero of=$FATIMG bs=1M count=$fat_size_mb
    fi

    # 格式化镜像文件为 FAT 文件系统
    echo "mkfs.fat -S $page_size $FATIMG"
    mkfs.fat -S $page_size $FATIMG

    # 将文件复制到镜像文件中
    #echo "MTOOLS_SKIP_CHECK=1 mcopy -bspmn -D s -i $FATIMG $resource_dir/* ::/"
    #MTOOLS_SKIP_CHECK=1 mcopy -bspmn -D s -i $FATIMG $resource_dir/* ::/
    copy_resource $FATIMG $resource_dir

    root_file_size=$(stat -c%s $FATIMG)
    if [ "$root_file_size" -gt "$root_size_byte" ]; then
        echo "Error: resource files is greater than root partition size $root_size_byte"
        echo "Please enlarge root partition size"
        echo "Error: generate $IMAGE/$ROOTIMG failed"
        rm $FATIMG
        rm $IMAGE/$ROOTIMG
        exit 1
    fi

    if [ ! -z "$target_file" ]; then
        cp $FATIMG $target_file
        echo "generate $target_file success"
        rm $FATIMG
        exit 0
    fi

    if [ -f "$FATIMG" ]; then
        echo "generate $IMAGE/$ROOTIMG success"
        cp $FATIMG $IMAGE/$ROOTIMG
        ls -l $IMAGE/$ROOTIMG
        rm $FATIMG
    else
        echo "Error: generate $IMAGE/$ROOTIMG failed"
    fi
}

make_userdata_fs() {
    parameter_file=$2
    resource_dir=$1
    flash_page_size_kb=$3
    flash_block_size_kb=$4
    flash_size_mb=$5
    IMAGE=Image
    DATAIMG=userdata.img
    FATIMG=fat.img

    if [ ! -f "$parameter_file" ]; then
        echo "Error: file not found: $parameter_file"
        exit 1
    fi

    if [ ! -d "$resource_dir" ]; then
        echo "Error: dir not found: $resource_dir"
        exit 1
    fi

    root_offset_sec=$(sed -n 's/.*-@0x\([0-9a-fA-F]\+\)(userdata:grow).*/\1/p' "$parameter_file")

    if [ -n "$root_offset_sec" ]; then
        decimal_value=$((0x$root_offset_sec))
        root_offset_byte=$((decimal_value * 512))
        echo "root_offset_sec=0x$root_offset_sec, root_offset_byte=$root_offset_byte"
    else
        echo "Error: userdata part not found in $parameter_file"
        exit 1
    fi

    flash_size_byte=$((flash_size_mb * 1024 * 1024))
    echo "flash_size_mb=$flash_size_mb, flash_size_byte=$flash_size_byte"

    flash_block_size_byte=$((flash_block_size_kb * 1024))
    echo "flash_block_size_kb=$flash_block_size_kb, flash_block_size_byte=$flash_block_size_byte"

    flash_page_size_byte=$((flash_page_size_kb * 1024))
    echo "flash_block_page_kb=$flash_page_size_kb, flash_page_size_byte=$flash_page_size_byte"

    if [ $# -eq 6 ]; then
        fat_size_mb=$6
        fat_size_byte=$(( fat_size_mb * 1024 * 1024  ))
    else
        # 计算 resource/* 文件的总大小
        total_size=$(du -c $resource_dir/* | grep total$ | awk '{print $1}')

        # 转换成MB并增加一定的余量，比如增加10%作为文件系统的元数据和空闲空间
        fat_size_mb=$(( (total_size / 1024) + 2 ))
        fat_size_mb=$(( fat_size_mb + (fat_size_mb / 10) ))

        fat_size_byte=$(( fat_size_mb * 1024 * 1024  ))

        echo "resource total_szie_kb=$total_size, resource fat_size_byte=$fat_size_byte, fat_size_mb=$fat_size_mb"
    fi
    fat_size_blocks=$(( fat_size_byte / flash_block_size_byte ))
    echo "userdata fat filesystm fat_size_byte=$fat_size_byte, fat_size_blocks=$fat_size_blocks"

    if [ "$fat_size_mb" -gt 8 ]; then
        flash_reserve_byte=$(( (fat_size_byte / 100 / flash_block_size_byte + 1) * flash_block_size_byte ))
    else
        flash_reserve_byte=$(( 2 * flash_block_size_byte ))
    fi
    echo "flash_reserve_byte=$flash_reserve_byte"

    root_size_byte=$(( flash_size_byte - root_offset_byte ))
    if [ "$root_size_byte" -lt "$flash_reserve_byte" ]; then
        echo "Error: root_size_byte=$root_size_byte < flash_reserve_byte=$flash_reserve_byte"
        exit 1
    fi
    root_size_byte=$(( root_size_byte - flash_reserve_byte ))
    root_size_blocks=$(( root_size_byte / flash_block_size_byte ))
    echo "root_size_byte=$root_size_byte, root_size_blocks=$root_size_blocks"

    echo "userdata fat filesystem size $fat_size_mb MB"
    if [ "$fat_size_byte" -gt "$root_size_byte" ]; then
        echo "Error: fat filesystem is $fat_size_byte greater than userdata partition size $root_size_byte"
        exit 1
    fi

    # 创建相应大小的镜像文件
    echo "dd if=/dev/zero of=$FATIMG bs=1M count=$fat_size_mb"
    dd if=/dev/zero of=$FATIMG bs=1M count=$fat_size_mb

    # 格式化镜像文件为 FAT 文件系统
    echo "mkfs.fat -S $flash_page_size_byte $FATIMG"
    mkfs.fat -S $flash_page_size_byte $FATIMG

    # 将文件复制到镜像文件中
    #echo "MTOOLS_SKIP_CHECK=1 mcopy -bspmn -D s -i $FATIMG $resource_dir/* ::/"
    #MTOOLS_SKIP_CHECK=1 mcopy -bspmn -D s -i $FATIMG $resource_dir/* ::/
    copy_resource $FATIMG $resource_dir

    # 执行 dhara_utils 工具进行处理
    echo "./rkbin/dhara_utils $FATIMG $flash_page_size_kb $fat_size_blocks"
    ./rkbin/dhara_utils $FATIMG $flash_page_size_kb $fat_size_blocks
    #./rkbin/dhara_utils $FATIMG 2 256

    if [ -f "$FATIMG.dhara" ]; then
        echo "generate $IMAGE/$DATAIMG success"
        cp $FATIMG.dhara $IMAGE/$DATAIMG
        ls -l $IMAGE/$DATAIMG
        rm $FATIMG
        rm $FATIMG.dhara
    else
        echo "Error: generate $IMAGE/$DATAIMG failed"
    fi
}

make_nand_dhara_fs() {
    resource_dir=$1
    target_file=$2
    flash_page_size_kb=$3
    flash_block_size_kb=$4
    partition_size_kb=$5
    FATIMG=fat.img

    if [ ! -d "$resource_dir" ]; then
        echo "Error: dir not found: $resource_dir"
        exit 1
    fi

    # Calculate the page size in bytes
    flash_page_size_byte=$((flash_page_size_kb * 1024))
    echo "flash_block_page_kb=$flash_page_size_kb, flash_page_size_byte=$flash_page_size_byte"

    # Calculate the blocks number of partitions
    partition_block_num=$((partition_size_kb / flash_block_size_kb))
    echo "partition_block_num=$partition_block_num"

    # Calculate the total size of the resource/*
    total_size_kb=$(du -c $resource_dir/* | grep total$ | awk '{print $1}')
    # Add 2M and 10% for filesystem metadata and free space
    fat_size_mb=$(( (total_size_kb / 1024) + 2 ))
    fat_size_mb=$(( fat_size_mb + (fat_size_mb / 10) ))

    # img currently occupies 72% of the total size
    img_size_mb=$(((partition_size_kb / 100) * 72 / 1024))

    if [ "$fat_size_mb" -gt "$img_size_mb" ]; then
        echo "Error: fat filesystem is $fat_size_mb greater than userdata partition size $img_size_mb"
        exit 1
    fi

    # Because rt-thread cannot resize filesystems, img is made equal to the partition size
    echo "dd if=/dev/zero of=$FATIMG bs=1M count=$img_size_mb"
    dd if=/dev/zero of=$FATIMG bs=1M count=$img_size_mb

    # Format the image file to the FAT file system
    echo "mkfs.vfat -S $flash_page_size_byte $FATIMG"
    mkfs.vfat -S $flash_page_size_byte $FATIMG

    # Copy the file to the image file
    copy_resource $FATIMG $resource_dir

    # Use dhara_utils to generate .img.dhara
    echo "../tools/dhara_utils $FATIMG $flash_page_size_kb $partition_block_num"
    ../tools/dhara_utils $FATIMG $flash_page_size_kb $partition_block_num

    if [ -f "$FATIMG.dhara" ]; then
        cp $FATIMG.dhara $target_file
        ls -l $target_file
        rm $FATIMG
        rm $FATIMG.dhara
        echo "generate $target_file success"
    else
        echo "Error: generate $target_file failed"
    fi
}

case $1 in
    root)
        make_root_fs $2 $3 $4 $5
        exit 0
        ;;
    nand)
        make_nand_dhara_fs $2 $3 $4 $5 $6
        exit 0
        ;;
    *)
        if [[ $# -ne 2 ]] && [[ $# -ne 5 ]] && [[ $# -ne 6 ]]; then
            usage
            usage_snand
            exit
        fi
esac

# ========================================= For Update.img =========================================

if [[ $# -eq 2 ]] && [[ $2 == *parameter* ]]; then
    make_root_fs $1 $2
    exit 0
fi
if [ $# -eq 6 ]; then
    make_userdata_fs $1 $2 $3 $4 $5 $6
    exit 0
fi
if [ $# -eq 5 ]; then
    make_userdata_fs $1 $2 $3 $4 $5
    exit 0
fi

# ========================================= For Firmware.img (only for spi nor flash)  =========================================

CUR_DIR=$(pwd)
IMAGE=$(pwd)/Image

ROOT_PATH="$1"
echo "$ROOT_PATH"
RESOURCE_PATH=$ROOT_PATH
ROOT_NAME=root.img
ROOTA_NAME=root_a.img
ROOTB_NAME=root_b.img

if [ ! -n "$2" ] ;then
FW_MERGER_CFG=$(pwd)/board/common/setting.ini
echo $FW_MERGER_CFG
else
FW_MERGER_CFG="$2"
echo $FW_MERGER_CFG
fi

#Del file whether exist
if [ ! -f "$IMAGE/$ROOT_NAME" ] || [ ! -f "$IMAGE/$ROOTA_NAME" ] || [ ! -f "$IMAGE/$ROOTB_NAME" ];then
    echo "root File not exist"
else
    rm -rf $IMAGE/root*.img
fi

ROOT_PART_SIZE=`grep -r -A 4 "^Name*=*root" $FW_MERGER_CFG |grep -wi "^PartSize" |cut -d '=' -f 2| cut -d 'x' -f 2`
echo "ROOT_PART_SIZE = $ROOT_PART_SIZE"

if [ -z "$ROOT_PART_SIZE" ] ; then
    echo "no root partition"
    exit 0
fi

#make_mnt_point $1

ROOT_CNT=0
# if [ $ROOT_CNT -eq 1 ]
# then
    # TRANS=`echo $ROOT_PART_SIZE | tr 'a-z' 'A-Z'`
    # ROOT_EACH_SIZE=`echo "ibase=16;$TRANS"| tr -d $'\r' | bc`
    # echo "userdata part size is :$ROOT_EACH_SIZE sector"
# fi

for each_size in $ROOT_PART_SIZE
do
    ROOT_CNT=$[$ROOT_CNT + 1]

    TRANS=`echo $each_size | tr 'a-z' 'A-Z'`
    ROOT_EACH_SIZE=`echo "ibase=16;$TRANS" |tr -d $'\r' | bc`
    echo "userdata part size is :$ROOT_EACH_SIZE sector"

    if check_root_is_last $FW_MERGER_CFG; then
        echo "root is not last partition"
    else
        echo "root is last partition"
        ROOT_EACH_SIZE=$(( ROOT_EACH_SIZE - 64))
        echo "userdata part size after reserve for gpt header is :$ROOT_EACH_SIZE sector"
    fi

    #transform to KB unit
    ROOT_PART_SIZE=`echo "ibase=10;$ROOT_EACH_SIZE/2" | bc`
    echo "userdata each part size is :$ROOT_PART_SIZE KB"

    #transform to 4096B one sector unit
    ROOT_SECTOR_SIZE=`echo "ibase=10;$ROOT_PART_SIZE/4" | bc`
    echo "userdata each part size is :$ROOT_SECTOR_SIZE *4KB"

    echo "Making $ROOT_NAME from $RESOURCE_PATH with size($ROOT_PART_SIZE K) with $ROOT_SECTOR_SIZE sectors"

    ROOT_NAME=root.img
    dd of=$ROOT_NAME bs=4K seek=$ROOT_SECTOR_SIZE count=0 2>&1 || fatal "Failed to dd image!"
    mkfs.fat -S 4096 $ROOT_NAME
    #MTOOLS_SKIP_CHECK=1 mcopy -bspmn -D s -i $ROOT_NAME $RESOURCE_PATH/* ::/
    copy_resource $ROOT_NAME $RESOURCE_PATH
done

if [ $ROOT_CNT -eq 2 ]
then
    for i in a b
    do
        cp -f $ROOT_NAME $IMAGE/root_$i.img
    done
    echo "remove $ROOT_NAME"
    rm -rf $ROOT_NAME
    echo "make root a/b image file done!"
else
    mv $ROOT_NAME $IMAGE
    echo "make root.image file done!"
fi
