#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"

[ "$RK_KERNEL_DTS" -a "$RK_KERNEL_DTB" -a -r "$RK_KERNEL_DTB" ] || exit 0

cd "$RK_SDK_DIR"

if [ "$RK_WIFIBT" ] && ! grep -wq wireless-bluetooth "$RK_KERNEL_DTB"; then
	echo -e "\e[35m"
	echo "Missing wireless-bluetooth in $RK_KERNEL_DTS(or .dtsi)!"
	echo -e "\e[0m"
	exit 1
fi

if [ "$RK_ROOTFS_TYPE" ] && [ "$RK_ROOTFS_TYPE" != ubi ] && \
	grep -q "rootfstype=" "$RK_KERNEL_DTB"; then
	ROOTFS_TYPE="$(strings "$RK_KERNEL_DTB" | grep -o "rootfstype=[^ ]*")"
	if [ "$ROOTFS_TYPE" != "rootfstype=$RK_ROOTFS_TYPE" ]; then
		echo -e "\e[35m"
		echo "Wrong $ROOTFS_TYPE in $RK_KERNEL_DTS(or .dtsi)!"
		echo "Expect: rootfstype=$RK_ROOTFS_TYPE"
		echo -e "\e[0m"
		exit 1
	fi
fi
