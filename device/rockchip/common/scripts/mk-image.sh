#!/bin/bash

# Prefer using buildroot host tools for compatible.
if [ -n "$RK_BUILDROOT_CFG" ]; then
    HOST_DIR="$RK_SDK_DIR/buildroot/output/$RK_BUILDROOT_CFG/host"
    export PATH=$HOST_DIR/usr/sbin:$HOST_DIR/usr/bin:$HOST_DIR/sbin:$HOST_DIR/bin:$PATH
    echo "Using host tools in $HOST_DIR (except for mke2fs)"
else
    "$(dirname "$(realpath "$0")")/check-mkimage.sh"
fi

fatal()
{
    echo -e "FATAL: " $@
    exit 1
}

usage()
{
    echo ${@:-"Wrong argumants"}
    echo "Usage: $0 [options] <source directory> <dest image>"
    echo "Options:"
    echo "-t, --type <type>    Filesystem type <ext4|msdos|...> (default is: ext4)"
    echo "-s, --size <size>    Filesystem size <size(M|K)|auto> (default is: auto)"
    echo "-l, --label <label>  Filesystem label"
    exit 1
}

unset SRC_DIR TARGET FS_TYPE SIZE LABEL
while true; do
    case "$1" in
        "")
            [ "$SRC_DIR" ] || usage "No source directory"
            [ "$TARGET" ] || usage "No target image"
            break
            ;;
        -t|--type)
            FS_TYPE=$2
            shift 2 || usage
            ;;
        -s|--size)
            SIZE=$2
            shift 2 || usage
            ;;
        -l|--label)
            LABEL=$2
            shift 2 || usage
            ;;
        *)
            if [ -z "$SRC_DIR" ]; then
                SRC_DIR=$1
                shift
            elif [ -z "$TARGET" ]; then
                TARGET=$1
                shift
            else
                usage
            fi
            ;;
    esac
done

[ "$FS_TYPE" ] || FS_TYPE=ext4
[ "$SIZE" ] || SIZE=auto

case $SIZE in
    auto)
        SIZE_KB=0
        ;;
    *K)
        SIZE_KB=$(( ${SIZE%K} ))
        ;;
    *G)
        SIZE_KB=$(( ${SIZE%G} * 1024 * 1024 ))
        ;;
    *)
        SIZE_KB=$(( ${SIZE%M} * 1024 )) # default is MB
        ;;
esac

echo $SIZE_KB | grep -vq [^0-9] || usage "Invalid size: $SIZE_KB"

TEMP=$(mktemp -u)

[ -d "$SRC_DIR" ] || usage "No such src dir: $SRC_DIR"

copy_to_ntfs()
{
    DEPTH=1
    while true;do
        find $SRC_DIR -maxdepth $DEPTH -mindepth $DEPTH -type d|grep -q "" \
            || break
        find $SRC_DIR -maxdepth $DEPTH -mindepth $DEPTH -type d \
            -exec sh -c 'ntfscp $TARGET "$1" "${1#$SRC_DIR}"' sh {} \; || \
            fatal "Detected non-buildroot ntfscp(doesn't support dir copy)"
                    DEPTH=$(($DEPTH + 1))
    done

    find $SRC_DIR -type f \
        -exec sh -c 'ntfscp $TARGET "$1" "${1#$SRC_DIR}"' sh {} \; || \
            fatal "Failed to do ntfscp!"
}

copy_to_image()
{
    ls $SRC_DIR/* &>/dev/null || return 0

    echo "Copying $SRC_DIR into $TARGET (root permission required)"
    mkdir -p $TEMP || return 1
    sudo mount $TARGET $TEMP || return 1

    cp -rp $SRC_DIR/* $TEMP
    RET=$?

    sudo umount $TEMP
    rm -rf $TEMP

    return $RET
}

check_host_tool()
{
    which $1|grep -wq buildroot
}

mkimage()
{
    echo "Making $TARGET from $SRC_DIR with size(${SIZE_KB}KB)"
    rm -rf $TARGET

    case $FS_TYPE in
        ext[234])
            /sbin/mke2fs -t $FS_TYPE $TARGET -d $SRC_DIR -b 4096 ${SIZE_KB}K \
                ${LABEL:+-L $LABEL} || return 1

            # Set max-mount-counts to 0, and disable the time-dependent checking.
            tune2fs -c 0 -i 0 $TARGET
            ;;
        msdos|fat|vfat)
            truncate -s ${SIZE_KB}K $TARGET

            # Use fat32 by default
            mkfs.vfat -F 32 ${LABEL:+-n $LABEL} $TARGET && MTOOLS_SKIP_CHECK=1 \
                mcopy -bspmn -D s -i $TARGET $SRC_DIR/* ::/
            ;;
        ntfs)
            truncate -s ${SIZE_KB}K $TARGET

            # Enable compression
            mkntfs -FCQ ${LABEL:+-L $LABEL} $TARGET
            if check_host_tool ntfscp; then
                copy_to_ntfs
            else
                copy_to_image
            fi
            ;;
        btrfs)
            truncate -s ${SIZE_KB}K $TARGET

            mkfs.btrfs ${LABEL:+-L $LABEL} -r $SRC_DIR $TARGET
            ;;
        f2fs)
            truncate -s ${SIZE_KB}K $TARGET

            mkfs.f2fs ${LABEL:+-l $LABEL} $TARGET
            sload.f2fs -f $SRC_DIR $TARGET
            ;;
        ubi|ubifs) mk_ubi_image ;;
    esac
}

mkimage_auto_sized()
{
    echo "Making $TARGET from $SRC_DIR (auto sized)"

    # Apparent size and maxium alignment(file_count * block_size)
    SIZE_KB="$(($(du --apparent-size -sk $SRC_DIR | cut -f 1) + \
        $(find $SRC_DIR | wc -l) * 4))"
    SIZE_KB="$((SIZE_KB + $SIZE_KB * 10 / 100))" # Start with extra 10%
    MAX_RETRY=20
    RETRY=0

    if [ "$FS_TYPE" = ext4 ] && [ "$SIZE_KB" -lt 8192 ]; then
        echo "Start from 8M for ext4."
        SIZE_KB=8192
    fi

    while true;do
        mkimage && break

        RETRY=$[RETRY+1]
        [ $RETRY -gt $MAX_RETRY ] && fatal "Failed to make image!"

        echo "Retring with increased size....($RETRY/$MAX_RETRY)"

        EXTRA_SIZE=$(($SIZE_KB / 50)) # Retry with extra 2%
        SIZE_KB=$(($SIZE_KB + ($EXTRA_SIZE > 4096 ? $EXTRA_SIZE : 4096)))
    done
}

mk_ubi_image()
{
    TARGET_DIR="${RK_OUTDIR:-$(dirname "$TARGET")}"
    UBI_VOL_NAME=${LABEL:-ubi}

    # default page size 2KB
    UBI_PAGE_SIZE=${RK_UBI_PAGE_SIZE:-2048}
    # default block size 128KB
    UBI_BLOCK_SIZE=${RK_UBI_BLOCK_SIZE:-0x20000}

    UBIFS_LEBSIZE=$(( $UBI_BLOCK_SIZE - 2 * $UBI_PAGE_SIZE ))
    UBIFS_MINIOSIZE=$UBI_PAGE_SIZE
    UBIFS_MAXLEBCNT=$(( $SIZE_KB * 1024 / $UBIFS_LEBSIZE ))

    UBIFS_IMAGE="$TARGET_DIR/$UBI_VOL_NAME.ubifs"
    UBINIZE_CFG="$TARGET_DIR/${UBI_VOL_NAME}-ubinize.cfg"

    mkfs.ubifs -x lzo -e $UBIFS_LEBSIZE -m $UBIFS_MINIOSIZE \
        -c $UBIFS_MAXLEBCNT -d $SRC_DIR -F -v -o $UBIFS_IMAGE || return 1

    echo "[ubifs]" > $UBINIZE_CFG
    echo "mode=ubi" >> $UBINIZE_CFG
    echo "vol_id=0" >> $UBINIZE_CFG
    echo "vol_type=dynamic" >> $UBINIZE_CFG
    echo "vol_name=$UBI_VOL_NAME" >> $UBINIZE_CFG
    echo "vol_alignment=1" >> $UBINIZE_CFG
    echo "vol_flags=autoresize" >> $UBINIZE_CFG
    echo "image=$UBIFS_IMAGE" >> $UBINIZE_CFG
    ubinize -o $TARGET -m $UBIFS_MINIOSIZE -p $UBI_BLOCK_SIZE \
        -v $UBINIZE_CFG
}

rm -rf $TARGET
case $FS_TYPE in
    ext[234]|msdos|fat|vfat|ntfs|btrfs|f2fs|ubi|ubifs)
        if [ $SIZE_KB -eq 0 ]; then
            mkimage_auto_sized || exit 1
        else
            mkimage || exit 1
        fi
        ;;
    erofs)
        [ $SIZE_KB -eq 0 ] || fatal "$FS_TYPE: fixed size not supported."
        mkfs.erofs -zlz4hc $TARGET $SRC_DIR|| exit 1
        ;;
    squashfs)
        [ $SIZE_KB -eq 0 ] || fatal "$FS_TYPE: fixed size not supported."
        mksquashfs $SRC_DIR $TARGET -noappend -comp lz4 || exit 1
        ;;
    jffs2)
        [ $SIZE_KB -eq 0 ] || fatal "$FS_TYPE: fixed size not supported."
        mkfs.jffs2 -r $SRC_DIR -o $TARGET 0x10000 \
            --pad=0x400000 -s 0x1000 -n || exit 1
        ;;
    *)
        usage "File system: $FS_TYPE not supported."
        exit 1
        ;;
esac

echo "Generated $TARGET"
