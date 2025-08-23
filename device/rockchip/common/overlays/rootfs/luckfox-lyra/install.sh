#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

[ "$RK_LUCKFOX_LYRA" ] || exit 0

$RK_RSYNC "$OVERLAY_DIR/etc" "$OVERLAY_DIR/usr" "$TARGET_DIR/"

install_sysv_service "$OVERLAY_DIR/S03modules_init.sh" S
install_sysv_service "$OVERLAY_DIR/S35wifibt-poweron.sh" S
install_sysv_service "$OVERLAY_DIR/S45usbconfig" S
install_sysv_service "$OVERLAY_DIR/S60netdevice" S
install_busybox_service "$OVERLAY_DIR/S03modules_init.sh"
install_busybox_service "$OVERLAY_DIR/S35wifibt-poweron.sh"
install_busybox_service "$OVERLAY_DIR/S45usbconfig"
install_busybox_service "$OVERLAY_DIR/S60netdevice"
