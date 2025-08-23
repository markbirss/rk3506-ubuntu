#!/bin/bash -e

build_wifibt()
{
	DEFAULT_ROOTFS_DIR="$RK_OUTDIR/rootfs/target"
	ROOTFS_DIR="${1:-$DEFAULT_ROOTFS_DIR}"
	if [ ! -d "$ROOTFS_DIR" ]; then
		error "$ROOTFS_DIR is not a dir!"
		return 1
	fi

	RK_WIFIBT_MODULES="${2:-$RK_WIFIBT_MODULES}"
	"$RK_SCRIPTS_DIR/post-wifibt.sh" "$(realpath "$ROOTFS_DIR")" \
		$([ -r "$ROOTFS_DIR/etc/os-release" ] || echo buildroot)
	finish_build build_wifibt $@
}

# Hooks

usage_hook()
{
	usage_oneline "wifibt[:<dst dir>[:<chip>]]" "build Wifi/BT"
}

BUILD_CMDS="wifibt"
build_hook()
{
	shift
	build_wifibt $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

build_wifibt $@
