#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

if [ "$RK_ROOTFS_INSTALL_MODULES" ]; then
	message "Installing kernel modules..."

	"$RK_SCRIPTS_DIR/mk-kernel.sh" modules "$TARGET_DIR/"
fi

if [ "$RK_ROOTFS_STRIP_MODULES" ]; then
	message "Strip kernel modules..."

	source "$RK_SCRIPTS_DIR/kernel-helper"

	find "$TARGET_DIR" -name "*.ko" \
		-exec ${RK_KERNEL_TOOLCHAIN}strip --strip-unneeded -v {} \;
fi
