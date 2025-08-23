#!/bin/bash -e

[ "$RK_ROOTFS_FRECON" ] || exit 0

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

message "Installing frecon to $TARGET_DIR..."
$RK_RSYNC "$OVERLAY_DIR/usr" "$OVERLAY_DIR/etc" "$TARGET_DIR/"

mkdir -p "$TARGET_DIR/etc/profile.d"
{
	echo "export FRECON_SHELL=$RK_ROOTFS_FRECON_SHELL"
	[ -z "$RK_ROOTFS_FRECON_VTS" ] || echo "export FRECON_VTS=1"
	[ -z "$RK_ROOTFS_FRECON_VT1" ] || echo "export FRECON_VT1=1"
	echo "export FRECON_FB_ROTATE=$RK_ROOTFS_FRECON_ROTATE"
	echo "export FRECON_FB_SCALE=$RK_ROOTFS_FRECON_SCALE"
	echo "export FRECON_OUTPUT_CONFIG=$RK_ROOTFS_FRECON_OUTPUT_CONFIG"
} > "$TARGET_DIR/etc/profile.d/frecon.sh"

install_sysv_service "$OVERLAY_DIR/S35frecon" 5 4 3 2 K02 0 1 6
install_busybox_service "$OVERLAY_DIR/S35frecon"
install_systemd_service "$OVERLAY_DIR/frecon.service" \
	"$OVERLAY_DIR/S35frecon" "/etc/init.d/frecon"
