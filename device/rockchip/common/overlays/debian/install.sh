#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

if [ "$RK_KERNEL_ARCH" = arm64 ]; then
	KBUILD_ARCH=aarch64
else
	KBUILD_ARCH=armhf
fi
HEADERS_TAR="$RK_OUTDIR/linux-headers/linux-headers-$KBUILD_ARCH.tar.gz"
HEADERS_DIR="$TARGET_DIR/usr/src/linux-headers-$RK_KERNEL_VERSION_RAW-rockchip"

"$RK_SCRIPTS_DIR/mk-kernel.sh" linux-headers $KBUILD_ARCH

message "Installing linux-headers to $HEADERS_DIR..."

mkdir -p "$HEADERS_DIR"
tar xf "$HEADERS_TAR" -C "$HEADERS_DIR"
