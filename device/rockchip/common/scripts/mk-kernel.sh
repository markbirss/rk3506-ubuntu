#!/bin/bash -e

KERNELS=$(ls | grep kernel- || true)

do_make_kernel_config()
{
	KERNEL_CONFIGS_DIR="$RK_SDK_DIR/kernel/arch/$RK_KERNEL_ARCH/configs"
	KERNEL_CFG="$1"
	shift
	KERNEL_CFG_FRAGMENTS="$@"

	if [ "$RK_CHIP" = "$RK_CHIP_FAMILY" ]; then
		POSSIBLE_FRAGMENTS="$RK_CHIP"
	elif echo "$RK_CHIP" | grep -qE "[0-9][a-z]$"; then
		POSSIBLE_FRAGMENTS="$RK_CHIP_FAMILY ${RK_CHIP%[a-z]} $RK_CHIP"
	else
		POSSIBLE_FRAGMENTS="$RK_CHIP_FAMILY $RK_CHIP"
	fi

	POSSIBLE_FRAGMENTS="$(echo "$POSSIBLE_FRAGMENTS" | xargs -n 1 | uniq | \
		sed "s/\(.*\)/\1.config \1_linux.config/")"

	unset BASIC_CFG_FRAGMENTS
	for cfg in $POSSIBLE_FRAGMENTS; do
		[ -r "$KERNEL_CONFIGS_DIR/$cfg" ] || continue

		message "# Found kernel's basic config fragment: $cfg"
		BASIC_CFG_FRAGMENTS="$BASIC_CFG_FRAGMENTS $cfg"
	done

	run_command $KMAKE $KERNEL_CFG $BASIC_CFG_FRAGMENTS $KERNEL_CFG_FRAGMENTS
}

make_kernel_config()
{
	do_make_kernel_config "$RK_KERNEL_CFG" "$RK_KERNEL_CFG_FRAGMENTS"
}

make_recovery_kernel_config()
{
	do_make_kernel_config "${RK_KERNEL_RECOVERY_CFG:-$RK_KERNEL_CFG}" \
		"${RK_KERNEL_RECOVERY_CFG_FRAGMENTS:-$RK_KERNEL_CFG_FRAGMENTS}"
}

do_build()
{
	check_config RK_KERNEL RK_KERNEL_CFG || false

	if [ "$DRY_RUN" ]; then
		notice "Commands of building $1:"
	else
		message "=========================================="
		message "          Start building $1"
		message "=========================================="
	fi

	make_kernel_config

	if [ -z "$DRY_RUN" ]; then
		"$RK_SCRIPTS_DIR/check-kernel.sh"
	fi

	case "$1" in
		kernel-config | kconfig)
			KERNEL_CONFIG_DIR="kernel/arch/$RK_KERNEL_ARCH/configs"
			run_command $KMAKE menuconfig
			run_command $KMAKE savedefconfig
			run_command mv kernel/defconfig \
				"$KERNEL_CONFIG_DIR/$RK_KERNEL_CFG"
			;;
		kernel-modules | modules)
			MOD_DIR="${2:-$RK_OUTDIR/kernel-modules}"
			run_command $KMAKE modules
			run_command $KMAKE modules_install \
				INSTALL_MOD_PATH="$MOD_DIR"
			run_command find "$MOD_DIR/lib/modules/" -type l -delete
			;;
		kernel*)
			run_command $KMAKE "$RK_KERNEL_DTS_NAME.img"

			# The FIT image for initrd would be packed in rootfs stage
			if [ -n "$RK_BOOT_FIT_ITS" ] && \
				[ -z "$RK_ROOTFS_INITRD" ]; then
				run_command "$RK_SCRIPTS_DIR/mk-fitimage.sh" \
					"kernel/$RK_BOOT_IMG" \
					"$RK_BOOT_FIT_ITS" \
					"$RK_KERNEL_IMG" "$RK_KERNEL_DTB" \
					"kernel/resource.img"
			fi

			if [ "$RK_SECURITY" ]; then
				if [ "$RK_SECURITY_CHECK_BASE" ]; then
					run_command \
						"$RK_SCRIPTS_DIR/mk-security.sh" \
						sign boot "kernel/$RK_BOOT_IMG" \
						$RK_FIRMWARE_DIR/
				fi
			else
				run_command ln -rsf "kernel/$RK_BOOT_IMG" \
					"$RK_FIRMWARE_DIR/boot.img"
			fi

			[ -z "$DRY_RUN" ] || return 0

			"$RK_SCRIPTS_DIR/check-kernel-dtb.sh"
			"$RK_SCRIPTS_DIR/check-power-domain.sh"
			"$RK_SCRIPTS_DIR/check-security.sh" kernel dts
			;;
	esac
}

build_recovery_kernel()
{
	check_config RK_KERNEL || false

	if [ "$DRY_RUN" ]; then
		notice "Commands of building $1:"
	else
		message "=========================================="
		message "          Start building $1"
		message "=========================================="
	fi

	KERNEL_DIR="$RK_SDK_DIR/kernel"
	RECOVERY_KERNEL_DIR="$RK_OUTDIR/recovery-kernel"
	RECOVERY_KERNEL_IMG="${RK_KERNEL_IMG#kernel/}"
	RECOVERY_KERNEL_DTB="${RK_KERNEL_DTS_DIR#kernel/}/${RK_KERNEL_RECOVERY_DTS_NAME:-$RK_KERNEL_DTS_NAME}.dtb"
	RECOVERY_KERNEL_DTB_TARGET="${RECOVERY_KERNEL_DTB##*/boot/dts/}"

	if [ -z "$RK_KERNEL_RECOVERY_CFG" ] && \
		[ -z "$RK_KERNEL_RECOVERY_CFG_FRAGMENTS" ] && \
		[ -z "$RK_KERNEL_RECOVERY_DTS_NAME" ] && \
		[ -z "$RK_KERNEL_RECOVERY_LOGO" ] && \
		[ -z "$RK_KERNEL_RECOVERY_LOGO_KERNEL" ]; then
		run_command rm -rf "$RECOVERY_KERNEL_DIR"
		run_command ln -rsf "$KERNEL_DIR" "$RECOVERY_KERNEL_DIR"
		run_command cd "$RECOVERY_KERNEL_DIR"

		make_kernel_config
		run_command $KMAKE "$(basename "$RECOVERY_KERNEL_IMG")"
		run_command $KMAKE "$RECOVERY_KERNEL_DTB_TARGET"
	else
		if [ ! -d "$RECOVERY_KERNEL_DIR" ] || \
			[ -L "$RECOVERY_KERNEL_DIR" ]; then
			run_command rm -rf "$RECOVERY_KERNEL_DIR"
			run_command mkdir -p "$RECOVERY_KERNEL_DIR"
		fi

		LOADER_LOGO="${RK_KERNEL_RECOVERY_LOGO:-logo.bmp}"
		KERNEL_LOGO="${RK_KERNEL_RECOVERY_LOGO_KERNEL:-logo_kernel.bmp}"

		run_command cd "$RECOVERY_KERNEL_DIR"
		run_command ln -rsf "$KERNEL_DIR/.git" .
		run_command ln -rsf "$KERNEL_DIR/$LOADER_LOGO" logo.bmp
		run_command ln -rsf "$KERNEL_DIR/$KERNEL_LOGO" logo_kernel.bmp
		run_command mkdir -p scripts
		run_command ln -rsf "$KERNEL_DIR/scripts/resource_tool" scripts/

		# HACK: Based on kernel/Makefile's MRPROPER_FILES
		run_command tar cf "$RK_OUTDIR/kernel.tar" \
			--remove-files --ignore-failed-read \
			"$KERNEL_DIR/.config" "$KERNEL_DIR/.config.old" \
			"$KERNEL_DIR/include/config" \
			"$KERNEL_DIR/include/generated" \
			"$KERNEL_DIR/arch/$RK_KERNEL_ARCH/include/generated" \
			"$KERNEL_DIR/Module.symvers"


		KMAKE="$KMAKE O=$RECOVERY_KERNEL_DIR"
		make_recovery_kernel_config
		run_command $KMAKE "$(basename "$RECOVERY_KERNEL_IMG")"
		run_command $KMAKE "$RECOVERY_KERNEL_DTB_TARGET"

		run_command tar xf "$RK_OUTDIR/kernel.tar" -C /
		run_command rm -f "$RK_OUTDIR/kernel.tar"
	fi

	run_command ln -rsf "$RECOVERY_KERNEL_IMG" \
		"$RK_OUTDIR/recovery-kernel.img"
	run_command ln -rsf "$RECOVERY_KERNEL_DTB" \
		"$RK_OUTDIR/recovery-kernel.dtb"
	run_command scripts/resource_tool "$RECOVERY_KERNEL_DTB" \
		logo.bmp logo_kernel.bmp
	run_command ln -rsf resource.img "$RK_OUTDIR/recovery-resource.img"
}

pack_linux_headers()
{
	case "${1:-host}" in
		host)
			case "$(uname -m)" in
				x86_64) KBUILD_ARCH=amd64 ;;
				i686|i386) KBUILD_ARCH=i386 ;;
				arm64|aarch64) KBUILD_ARCH=aarch64 ;;
				arm32|armhf) KBUILD_ARCH=armhf ;;
				*)
					warning "Unknown host arch: $(uname -m)!"
					return 0 ;;
			esac

			KBUILD_DIR="$RK_SDK_DIR/kernel"
			;;
		aarch64|armhf)
			KBUILD_ARCH=$1
			KBUILD_DIR="$RK_KBUILD_DIR/$KBUILD_ARCH/linux-kbuild-$RK_KERNEL_VERSION_RAW"
			;;
		*) return 0 ;;
	esac
	HEADERS_OUTDIR="$RK_OUTDIR/linux-headers"
	HEADERS_TAR="$HEADERS_OUTDIR/linux-headers-$KBUILD_ARCH.tar"
	HEADERS_KBUILD_DIR="$HEADERS_OUTDIR/linux-kbuild-$KBUILD_ARCH"
	HEADERS_PACK_SCRIPT="$(mktemp)"

	if [ "$DRY_RUN" ]; then
		notice "Commands of packing linux-headers-$KBUILD_ARCH:"
	else
		message "=========================================="
		message "          Start packing linux-headers-$KBUILD_ARCH"
		message "=========================================="
	fi

	run_command mkdir -p "$HEADERS_OUTDIR"
	run_command rm -rf "$HEADERS_KBUILD_DIR" "$HEADERS_TAR"*
	run_command ln -rsf "$KBUILD_DIR" "$HEADERS_KBUILD_DIR"

	cat << EOF > "$HEADERS_PACK_SCRIPT"
	{
		# Based on kernel/scripts/package/builddeb (6.1)
		find . arch/$RK_KERNEL_ARCH -maxdepth 1 -name Makefile\*
		find include scripts -type f -o -type l
		find arch/$RK_KERNEL_ARCH -name module.lds -o -name Kbuild.platforms -o -name Platform
		find \$(find arch/$RK_KERNEL_ARCH -name include -o -name scripts -type d) -type f
		find arch/$RK_KERNEL_ARCH/include Module.symvers include scripts -type f
		echo .config
	} | tar --no-recursion --ignore-failed-read -T - -cf "$HEADERS_TAR"

	# Pack kbuild
	tar -rf "$HEADERS_TAR" -C "$HEADERS_KBUILD_DIR" scripts/ tools/
EOF

	run_command cd "$RK_SDK_DIR/kernel"
	cat "$HEADERS_PACK_SCRIPT"
	if [ -z "$DRY_RUN" ]; then
		. "$HEADERS_PACK_SCRIPT"
	fi
	run_command cd "$RK_SDK_DIR"
	rm -f "$HEADERS_PACK_SCRIPT"

	[ -z "$DRY_RUN" ] || return 0

	# Packing .deb package
	KERNELRELEASE="$($KMAKE --no-print-directory kernelrelease)"
	TEMP_DIR="$(mktemp -d)"
	DEBIAN_ARCH="${KBUILD_ARCH/aarch64/arm64}"
	DEBIAN_PKG="linux-headers-${RK_KERNEL_VERSION_RAW}-$RK_KERNEL_ARCH"
	KERNEL_HEADERS_DIR="/usr/src/linux-headers-$KERNELRELEASE"
	DEBIAN_DIR="$TEMP_DIR/${DEBIAN_PKG}_$DEBIAN_ARCH"
	DEBIAN_HEADERS_DIR="$DEBIAN_DIR/$KERNEL_HEADERS_DIR"
	DEBIAN_DEB="$DEBIAN_DIR.deb"
	DEBIAN_CONTROL="$DEBIAN_DIR/DEBIAN/control"

	message "Unpacking $HEADERS_TAR ..."

	mkdir -p "$DEBIAN_HEADERS_DIR"
	tar xf "$HEADERS_TAR" -C "$DEBIAN_HEADERS_DIR"

	KERNEL_MODULES_DIR="/lib/modules/$KERNELRELEASE"

	mkdir -p "$DEBIAN_DIR/$KERNEL_MODULES_DIR"
	ln -sf "$KERNEL_HEADERS_DIR" "$DEBIAN_DIR/$KERNEL_MODULES_DIR/build"
	ln -sf "$KERNEL_HEADERS_DIR" "$DEBIAN_DIR/$KERNEL_MODULES_DIR/source"

	mkdir -p "$(dirname "$DEBIAN_CONTROL")"
	cat << EOF > "$DEBIAN_CONTROL"
Package: $DEBIAN_PKG
Source: linux-rockchip ($RK_KERNEL_VERSION_RAW)
Version: $RK_KERNEL_VERSION_RAW-rockchip
Architecture: $DEBIAN_ARCH
Section: kernel
Priority: optional
Multi-Arch: foreign
Maintainer: Tao Huang <huangtao@rock-chips.com>
Homepage: https://www.kernel.org/
Description: Kbuild and headers for Rockchip Linux $RK_KERNEL_VERSION_RAW $RK_KERNEL_ARCH configuration
EOF

	message "Debian control file:"
	cat "$DEBIAN_CONTROL"

	message "Packing $(basename "$DEBIAN_DEB")..."
	dpkg-deb -b "$DEBIAN_DIR"
	mv "$DEBIAN_DEB" "$HEADERS_OUTDIR"

	rm -rf "$TEMP_DIR"

	gzip "$HEADERS_TAR"

	finish_build linux-headers-$KBUILD_ARCH
}

# Hooks

usage_hook()
{
	for k in $KERNELS; do
		usage_oneline "$k[:dry-run]" "build kernel ${k#kernel-}"
	done

	usage_oneline "kernel[:dry-run]" "build kernel"
	usage_oneline "recovery-kernel[:dry-run]" "build kernel for recovery"
	usage_oneline "kernel-modules[:<dst dir>:dry-run]" "build kernel modules"
	usage_oneline "modules[:<dst dir>:dry-run]" "alias of kernel-modules"
	usage_oneline "linux-headers[:<arch>:dry-run]" "build linux-headers"
	usage_oneline "kernel-config[:dry-run]" "modify kernel defconfig"
	usage_oneline "kconfig[:dry-run]" "alias of kernel-config"
	usage_oneline "kernel-make[:<arg1>:<arg2>]" "run kernel make"
	usage_oneline "kmake[:<arg1>:<arg2>]" "alias of kernel-make"
}

clean_hook()
{
	[ ! -d kernel ] || make -C kernel distclean

	rm -rf "$RK_OUTDIR/recovery-*"
	rm -f "$RK_FIRMWARE_DIR/linux-headers.tar"
	rm -rf "$RK_FIRMWARE_DIR/boot.img"
}

INIT_CMDS="default $KERNELS"
init_hook()
{
	load_config RK_KERNEL_CFG
	check_config RK_KERNEL_CFG &>/dev/null || return 0

	local KERNEL_LAST="$(cat "$RK_OUTDIR/.kernel" 2>/dev/null || true)"
	local KERNEL_CURRENT="$(kernel_version)"

	load_config RK_KERNEL_PREFERRED

	# Priority: cmdline > env > last selected > preferred > current symlink
	if echo $1 | grep -q "^kernel-"; then
		export RK_KERNEL_VERSION=${1#kernel-}
		notice "Using kernel version($RK_KERNEL_VERSION) from cmdline"
	elif [ "$RK_KERNEL_VERSION" ]; then
		export RK_KERNEL_VERSION=${RK_KERNEL_VERSION//\"/}
		notice "Using kernel version($RK_KERNEL_VERSION) from environment"
	elif [ "$KERNEL_LAST" ]; then
		export RK_KERNEL_VERSION=$KERNEL_LAST
		notice "Using last kernel version($RK_KERNEL_VERSION)"
	elif [ "$RK_KERNEL_PREFERRED" ]; then
		export RK_KERNEL_VERSION=$RK_KERNEL_PREFERRED
		notice "Using preferred kernel version($RK_KERNEL_VERSION)"
	elif [ "$KERNEL_CURRENT" ]; then
		RK_KERNEL_VERSION=$KERNEL_CURRENT
		notice "Using current kernel version($RK_KERNEL_VERSION)"
	else
		RK_KERNEL_VERSION=5.10
		notice "Fallback to kernel version($RK_KERNEL_VERSION)"
	fi

	# Save the selected version
	echo "$RK_KERNEL_VERSION" > "$RK_OUTDIR/.kernel"

	[ "$RK_KERNEL_VERSION" != "$KERNEL_CURRENT" ] || return 0

	# Update kernel
	KERNEL_DIR=kernel-$RK_KERNEL_VERSION
	notice "\nSwitching to $KERNEL_DIR"
	if [ ! -d "$KERNEL_DIR" ]; then
		error "$KERNEL_DIR not exist!"
		exit 1
	fi

	rm -rf kernel
	ln -rsf $KERNEL_DIR kernel
}

PRE_BUILD_CMDS="kernel-config kconfig kernel-make kmake"
pre_build_hook()
{
	check_config RK_KERNEL RK_KERNEL_CFG || false
	source "$RK_SCRIPTS_DIR/kernel-helper"

	message "Toolchain for kernel:"
	message "${RK_KERNEL_TOOLCHAIN:-gcc}"
	echo

	case "$1" in
		kernel-make | kmake)
			if [ "$DRY_RUN" ]; then
				notice "Commands of building ${@:-stuff}:"
			else
				message "=========================================="
				message "          Start building $@"
				message "=========================================="
			fi

			shift

			if [ ! -r kernel/.config ]; then
				make_kernel_config
			fi
			run_command $KMAKE $@
			;;
		kernel-config | kconfig)
			do_build $@
			;;
	esac

	if [ -z "$DRY_RUN" ]; then
		finish_build $@
	fi
}

pre_build_hook_dry()
{
	DRY_RUN=1 pre_build_hook $@
}

BUILD_CMDS="$KERNELS kernel recovery-kernel kernel-modules modules"
build_hook()
{
	check_config RK_KERNEL RK_KERNEL_CFG || false
	source "$RK_SCRIPTS_DIR/kernel-helper"

	message "Toolchain for kernel:"
	message "${RK_KERNEL_TOOLCHAIN:-gcc}"
	echo

	case "$1" in
		recovery-kernel) build_recovery_kernel $@ ;;
		kernel-*)
			if [ "$RK_KERNEL_VERSION" != "${1#kernel-}" ] &&
				echo $KERNELS | grep -wq "$1"; then
				warning "Kernel version ${1#kernel-} ignored"
			fi
			;&
		*) do_build $@ ;;
	esac

	finish_build build_$1
}

build_hook_dry()
{
	DRY_RUN=1 build_hook $@
}

POST_BUILD_CMDS="linux-headers"
post_build_hook()
{
	[ "$1" = "linux-headers" ] || return 0
	shift

	check_config RK_KERNEL RK_KERNEL_CFG || false
	source "$RK_SCRIPTS_DIR/kernel-helper"

	if [ "$DRY_RUN" ]; then
		notice "Commands of building linux-headers:"
	else
		message "=========================================="
		message "          Start building linux-headers"
		message "=========================================="
	fi

	# Preparing kernel for linux-headers
	make_kernel_config
	run_command $KMAKE Image modules_prepare

	if [ -z "$DRY_RUN" ]; then
		"$RK_SCRIPTS_DIR/check-kernel-headers.sh"
	fi

	if [ "$1" ]; then
		pack_linux_headers "$1"
	else
		pack_linux_headers host
		pack_linux_headers armhf
		[ "$RK_CHIP_ARM32" ] || pack_linux_headers aarch64
	fi

	finish_build linux-headers
}

post_build_hook_dry()
{
	DRY_RUN=1 post_build_hook $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

case "${1:-kernel}" in
	kernel-config | kconfig | kernel-make | kmake) pre_build_hook "$@" ;;
	kernel* | recovery-kernel | kernel-modules | modules)
		init_hook "$@"
		build_hook "${@:-kernel}"
		;;
	linux-headers) post_build_hook "$@" ;;
	*) usage ;;
esac
