#!/bin/bash -e

POST_ROOTFS_ONLY=1

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

[ "$RK_ROOTFS_ASYNC_COMMIT" ] || exit 0

message "Installing async-commit service..."

rm -f etc/init.d/S*_commit.sh \
	etc/systemd/system/multi-user.target.wants/async.service \
	usr/lib/systemd/system/async.service

cd "$RK_SDK_DIR"

mkdir -p "$TARGET_DIR/usr/bin"
install -m 0755 external/rkscript/async-commit "$TARGET_DIR/usr/bin/"

ensure_tools "$TARGET_DIR/usr/bin/modetest"

install_sysv_service external/rkscript/S*async-commit.sh S
install_busybox_service external/rkscript/S*async-commit.sh
install_systemd_service external/rkscript/async-commit.service
