#!/bin/bash -e

[ "$RK_ROOTFS_BOOTANIM" ] || exit 0

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

message "Installing bootanim to $TARGET_DIR..."

cd "$RK_SDK_DIR"

mkdir -p "$TARGET_DIR/usr/bin"
install -m 0755 external/rkscript/bootanim "$TARGET_DIR/usr/bin/"
sed -i "s/^\(TIMEOUT=\).*/\1$RK_ROOTFS_BOOTANIM_TIMEOUT/" \
	"$TARGET_DIR/usr/bin/bootanim"

install_sysv_service external/rkscript/S*bootanim.sh S
install_busybox_service external/rkscript/S*bootanim.sh
install_systemd_service external/rkscript/bootanim.service \
	external/rkscript/S31bootanim.sh /etc/init.d/bootanim

rm -rf "$TARGET_DIR/etc/bootanim.d"
$RK_RSYNC "$OVERLAY_DIR/etc" "$TARGET_DIR/"
