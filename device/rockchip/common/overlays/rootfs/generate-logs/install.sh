#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

[ "$RK_ROOTFS_GENERATE_LOGS" ] || exit 0

$RK_RSYNC "$OVERLAY_DIR/etc" "$OVERLAY_DIR/usr" "$TARGET_DIR/"
