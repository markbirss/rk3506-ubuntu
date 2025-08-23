#!/bin/bash -e

POST_ROOTFS_ONLY=1

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

[ "$RK_ROOTFS_LOG_GUARDIAN" ] || exit 0

message "Installing log-guardian..."

cd "$RK_SDK_DIR"

mkdir -p "$TARGET_DIR/usr/bin"
install -m 0755 external/rkscript/log-guardian "$TARGET_DIR/usr/bin/"

sed -i -e "s/^\(INTERVAL=\).*/\1\"$RK_ROOTFS_LOG_GUARDIAN_INTERVAL\"/" \
	-e "s/^\(MIN_AVAIL_SIZE=\).*/\1\"$RK_ROOTFS_LOG_GUARDIAN_MIN_SIZE\"/" \
	-e "s#^\(LOG_DIRS=\).*#\1\"$RK_ROOTFS_LOG_GUARDIAN_LOG_DIRS\"#" \
	"$TARGET_DIR/usr/bin/log-guardian"

install_sysv_service external/rkscript/S*log-guardian.sh S
install_busybox_service external/rkscript/S*log-guardian.sh
install_systemd_service external/rkscript/log-guardian.service
