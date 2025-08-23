#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"

TARGET_DIR="$1"
TARGET_IMG="$TARGET_DIR/ramboot.img"
RAMDISK_IMG="$2"
ITS="$3"
KERNEL_IMG="${4:-$RK_SDK_DIR/$RK_KERNEL_IMG}"
KERNEL_DTB="${5:-$RK_SDK_DIR/$RK_KERNEL_DTB}"
SECOND_IMG="${6:-$RK_SDK_DIR/kernel/resource.img}"

if [ ! -r "$RAMDISK_IMG" ]; then
	echo "Need $RAMDISK_IMG to pack $TARGET_IMG"
	exit 1
fi

if [ ! -r "$KERNEL_IMG" ] || [ ! -r "$KERNEL_DTB" ] || \
	[ ! -r "$SECOND_IMG" ]; then
	echo -e "\e[35m"
	echo "Build kernel for packing $TARGET_IMG"
	echo -e "\e[0m"

	"$RK_SCRIPTS_DIR/mk-kernel.sh"
	KERNEL_IMG="$RK_KERNEL_IMG"
	KERNEL_DTB="$RK_KERNEL_DTB"
	SECOND_IMG="$RK_SDK_DIR/kernel/resource.img"
fi

mkdir -p "$TARGET_DIR"
cd "$TARGET_DIR"

ln -rsf "$RAMDISK_IMG" ramdisk.img
ln -rsf "$KERNEL_IMG" kernel.img
ln -rsf "$KERNEL_DTB" kernel.dtb
ln -rsf "$SECOND_IMG" second.img

# Force using compressed ramdisk
case "$(realpath ramdisk.img)" in
	*.gz | *.bz | *.bz2 | *.xz | *.squashfs) ;;
	*)
		ln -f ramdisk.img ramdisk.img.orig
		gzip -f -9 ramdisk.img
		ln -sf ramdisk.img.gz ramdisk.img
		;;
esac

echo "Packing $TARGET_IMG..."

if [ -r "$ITS" ]; then
	ln -rsf "$ITS" ramboot.its
	"$RK_SCRIPTS_DIR/mk-fitimage.sh" ramboot.img ramboot.its kernel.img \
		kernel.dtb second.img ramdisk.img
else
	"$RK_SDK_DIR/kernel/scripts/mkbootimg" --kernel kernel.img \
		--ramdisk ramdisk.img --second second.img -o ramboot.img
fi
