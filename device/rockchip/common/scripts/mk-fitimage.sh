#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"

TARGET_IMG="$1"
ITS="$2"
KERNEL_IMG="$3"
KERNEL_DTB="$4"
RESOURCE_IMG="$5"
RAMDISK_IMG="$6"

if [ ! -f "$ITS" ]; then
	echo "$ITS not exists!"
	exit 1
fi

TMP_ITS=$(mktemp)
cp "$ITS" "$TMP_ITS"

if [ "$RK_SECURITY" ]; then
	echo "Security boot enabled, removing uboot-ignore ..."
	sed -i "/uboot-ignore/d" "$TMP_ITS"
fi

sed -i -e "s~@KERNEL_DTB@~$(realpath -q "$KERNEL_DTB")~" \
	-e "s~@KERNEL_IMG@~$(realpath -q "$KERNEL_IMG")~" \
	-e "s~@RAMDISK_IMG@~$(realpath -q "$RAMDISK_IMG")~" \
	-e "s~@RESOURCE_IMG@~$(realpath -q "$RESOURCE_IMG")~" "$TMP_ITS"

"$RK_SDK_DIR/rkbin/tools/mkimage" -f "$TMP_ITS"  -E -p 0x800 "$TARGET_IMG"

rm -f "$TMP_ITS"
