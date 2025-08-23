#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

[ "$RK_ROOTFS_LD_CACHE" ] || exit 0

if [ -r "$TARGET_DIR/etc/ld.so.cache" ]; then
	notice "Keep original /etc/ld.so.cache"
	exit 0
fi

if ! grep -q glibc-ld.so.cache "$TARGET_DIR"/lib/ld-linux* &>/dev/null; then
	notice "glibc's ld.so.cache is unsupported"
	exit 0
fi

message "Creating ld.so.cache for $TARGET ..."

"$RK_TOOLS_DIR/x86_64/ldconfig" -r "$TARGET_DIR/"
