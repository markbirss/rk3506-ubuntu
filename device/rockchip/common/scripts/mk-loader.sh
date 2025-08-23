#!/bin/bash -e

build_uefi()
{
	check_config RK_KERNEL_DTS_NAME || false

	if [ "$RK_CHIP" != rk3588 -o ! -d uefi ]; then
		error "UEFI not supported!"
		return 1
	fi

	if [ ! -f "$RK_KERNEL_DTB" ]; then
		error "$RK_KERNEL_DTB not exists!"
		return 1
	fi

	UEFI_DIR=uefi/edk2-platforms/Platform/Rockchip/DeviceTree

	run_command cp "$RK_KERNEL_DTB" $UEFI_DIR/$RK_CHIP.dtb
	run_command cd uefi
	run_command $UMAKE $RK_UBOOT_CFG
}

build_uboot()
{
	check_config RK_LOADER RK_UBOOT_CFG || false

	if [ -z "$DRY_RUN" ]; then
		rm -f u-boot/*.bin u-boot/*.img

		"$RK_SCRIPTS_DIR/check-loader.sh"
	fi

	UARGS_COMMON="$RK_UBOOT_OPTS \
		${RK_UBOOT_INI:+../rkbin/RKBOOT/$RK_UBOOT_INI} \
		${RK_UBOOT_TRUST_INI:+../rkbin/RKTRUST/$RK_UBOOT_TRUST_INI}"
	UARGS="$UARGS_COMMON ${RK_UBOOT_SPL:+--spl-new}"

	[ ! "$RK_SECURITY_BURN_KEY" ] || \
		UARGS="$UARGS ${RK_SECUREBOOT_FIT:+--burn-key-hash}"

	run_command cd u-boot

	run_command $UMAKE $RK_UBOOT_CFG $RK_UBOOT_CFG_FRAGMENTS $UARGS
	[ ! -z "$DRY_RUN" ] || "$RK_SCRIPTS_DIR/check-security.sh" uboot

	if [ "$RK_SECURITY_OPTEE_STORAGE_SECURITY" ]; then
		if [ -z "$(rk_partition_size security)" ]; then
			error "\"security\" partition not found in parameter"
			return 1
		fi
	fi

	if [ "$RK_SECUREBOOT_AVB" ]; then
		if [ -z "$(rk_partition_size vbmeta)" ]; then
			error "\"vbmeta\" partition not found in parameter"
			return 1
		fi
	fi

	if [ "$RK_UBOOT_SPL" ]; then
		if [ "$DRY_RUN" ] || \
			! grep -q "ROCKCHIP_FIT_IMAGE_PACK=y" .config; then
			# Repack SPL for non-FIT u-boot
			run_command $UMAKE $UARGS_COMMON --spl
		fi
	fi

	if [ "$RK_UBOOT_RAW" ]; then
		run_command $UMAKE $UARGS_COMMON --idblock
	fi

	run_command cd ..

	if [ "$DRY_RUN" ]; then
		return 0
	fi

	LOADER="$(echo u-boot/*_loader_*.bin | head -1)"
	if [ "$RK_SECUREBOOT_AVB" ]; then
	       "$RK_SCRIPTS_DIR/mk-security.sh" sign loader $LOADER \
				"$RK_FIRMWARE_DIR"/MiniLoaderAll.bin
	       "$RK_SCRIPTS_DIR/mk-security.sh" sign uboot u-boot/uboot.img \
				"$RK_FIRMWARE_DIR"/uboot.img
	       "$RK_SCRIPTS_DIR/mk-security.sh" sign trust u-boot/trust.img \
				"$RK_FIRMWARE_DIR"/trust.img
	else
		ln -rsf "$LOADER" "$RK_FIRMWARE_DIR"/MiniLoaderAll.bin
		ln -rsf u-boot/uboot.img "$RK_FIRMWARE_DIR"
		[ ! -e u-boot/trust.img ] || \
			ln -rsf u-boot/trust.img "$RK_FIRMWARE_DIR"
	fi
}

# Hooks

usage_hook()
{
	usage_oneline "loader[:dry-run]" "build loader (u-boot)"
	usage_oneline "uboot[:dry-run]" "build u-boot"
	usage_oneline "u-boot[:dry-run]" "alias of uboot"
	usage_oneline "uefi[:dry-run]" "build uefi"
}

clean_hook()
{
	make -C u-boot distclean

	rm -rf "$RK_FIRMWARE_DIR/uboot.img"
	rm -rf "$RK_FIRMWARE_DIR/MiniLoaderAll.bin"
}

BUILD_CMDS="loader uboot u-boot uefi"
build_hook()
{
	if echo $RK_UBOOT_CFG $RK_UBOOT_CFG_FRAGMENTS | grep -q aarch32 && \
		[ "$RK_UBOOT_ARCH" = arm64 ]; then
		error "Wrong u-boot arch ($RK_UBOOT_ARCH) for config:" \
			"$RK_UBOOT_CFG $RK_UBOOT_CFG_FRAGMENTS\n"
		export RK_UBOOT_ARCH=arm
	fi

	RK_UBOOT_TOOLCHAIN="$(get_toolchain U-Boot "$RK_UBOOT_ARCH")"
	[ "$RK_UBOOT_TOOLCHAIN" ] || exit 1

	message "Toolchain for loader (U-Boot):"
	message "${RK_UBOOT_TOOLCHAIN:-gcc}"
	echo

	export UMAKE="./make.sh CROSS_COMPILE=$RK_UBOOT_TOOLCHAIN"

	if [ "$DRY_RUN" ]; then
		notice "Commands of building $1:"
	else
		message "=========================================="
		message "          Start building $1"
		message "=========================================="
	fi

	TARGET="$1"
	shift

	case "$TARGET" in
		uboot | u-boot | loader) build_uboot $@ ;;
		uefi) build_uefi $@ ;;
		*) usage ;;
	esac

	if [ -z "$DRY_RUN" ]; then
		finish_build build_$TARGET $@
	fi
}

build_hook_dry()
{
	DRY_RUN=1 build_hook $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

build_hook ${@:-loader}
