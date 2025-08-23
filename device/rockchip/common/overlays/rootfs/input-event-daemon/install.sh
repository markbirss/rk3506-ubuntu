#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

if [ -z "$RK_ROOTFS_INPUT_EVENT_DAEMON" ]; then
	notice "Disabling input-event-daemon..."
	find "$TARGET_DIR/etc" "$TARGET_DIR/lib" "$TARGET_DIR/usr" \
		-name "*input-event-daemon*" -print0 2>/dev/null | \
		xargs -0 rm -rf
	exit 0
fi

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

message "Installing input-event-daemon to $TARGET_DIR..."

mkdir -p "$TARGET_DIR/etc" "$TARGET_DIR/lib" "$TARGET_DIR/usr/bin"

# Conflict with triggerhappy
find "$TARGET_DIR/etc" "$TARGET_DIR/lib" "$TARGET_DIR/usr/bin" \
	-name "*triggerhappy*" -print0 | xargs -0 rm -rf

$RK_RSYNC "$OVERLAY_DIR/usr" "$OVERLAY_DIR/etc" "$TARGET_DIR/"

# Use inotifywait to monitor power state changes
ensure_tools "$TARGET_DIR/usr/bin/inotifywait"

install_sysv_service "$OVERLAY_DIR/S99input-event-daemon" 5 4 3 2 K01 0 1 6
install_busybox_service "$OVERLAY_DIR/S99input-event-daemon"
install_systemd_service "$OVERLAY_DIR/input-event-daemon.service"
