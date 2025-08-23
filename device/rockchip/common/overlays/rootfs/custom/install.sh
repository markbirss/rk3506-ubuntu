#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

if [ ! -d "" ]; then
	mkdir -p $OVERLAY_DIR/root
fi

$RK_RSYNC "$OVERLAY_DIR/root/" "$TARGET_DIR/"
