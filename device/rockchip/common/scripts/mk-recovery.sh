#!/bin/bash -e

usage_hook()
{
	usage_oneline "recovery" "build recovery"
}

clean_hook()
{
	if check_config RK_BUILDROOT &>/dev/null; then
		rm -rf buildroot/output/$RK_RECOVERY_CFG
	fi
	rm -rf "$RK_OUTDIR/recovery"

	rm -rf "$RK_FIRMWARE_DIR/recovery.img"
}

BUILD_CMDS="recovery"
build_hook()
{
	check_config RK_RECOVERY || false

	message "=========================================="
	message "          Start building recovery(buildroot)"
	message "=========================================="


	DST_DIR="$RK_OUTDIR/recovery"
	IMAGE_DIR="$DST_DIR/images"

	"$RK_SCRIPTS_DIR/mk-buildroot.sh" $RK_RECOVERY_CFG "$IMAGE_DIR"

	"$RK_SCRIPTS_DIR/mk-kernel.sh" recovery-kernel

	"$RK_SCRIPTS_DIR/mk-ramboot.sh" "$DST_DIR" \
		"$IMAGE_DIR/rootfs.$RK_RECOVERY_INITRD_TYPE" \
		"$RK_RECOVERY_FIT_ITS" "$RK_OUTDIR/recovery-kernel.img" \
		"$RK_OUTDIR/recovery-kernel.dtb" \
		"$RK_OUTDIR/recovery-resource.img"

	if [ "$RK_SECURITY" ]; then
		"$RK_SCRIPTS_DIR/mk-security.sh" sign recovery \
			$DST_DIR/ramboot.img $RK_FIRMWARE_DIR
	else
		ln -rsf "$DST_DIR/ramboot.img" "$RK_FIRMWARE_DIR/recovery.img"
	fi

	finish_build build_recovery
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

build_hook $@
