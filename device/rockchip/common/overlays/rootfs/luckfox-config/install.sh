#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

[ "$RK_LUCKFOX_CONFIG" ] || exit 0

$RK_RSYNC "$OVERLAY_DIR/usr" "$TARGET_DIR/"

install_sysv_service "$OVERLAY_DIR/S99luckfoxconfigload" S
install_busybox_service "$OVERLAY_DIR/S99luckfoxconfigload"