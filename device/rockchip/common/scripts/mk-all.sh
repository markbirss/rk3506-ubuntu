#!/bin/bash -e

BOARD=$(echo ${RK_KERNEL_DTS_NAME:-$(echo "$RK_DEFCONFIG" | \
	sed -n "s/.*\($RK_CHIP.*\)_defconfig/\1/p")} | \
	tr '[:lower:]' '[:upper:]')

build_all()
{
	message "=========================================="
	message "          Start building all images"
	message "=========================================="

	rm -rf "$RK_FIRMWARE_DIR"
	mkdir -p "$RK_FIRMWARE_DIR"

	"$RK_SCRIPTS_DIR/check-security.sh" keys

	[ -z "$RK_MISC" ] || "$RK_SCRIPTS_DIR/mk-misc.sh"
	[ -z "$RK_LOADER" ] || "$RK_SCRIPTS_DIR/mk-loader.sh"
	[ -z "$RK_KERNEL" ] || "$RK_SCRIPTS_DIR/mk-kernel.sh"
	[ -z "$RK_ROOTFS" ] || "$RK_SCRIPTS_DIR/mk-rootfs.sh"
	[ -z "$RK_SECURITY_INITRD_CFG" ] || \
		"$RK_SCRIPTS_DIR/mk-security.sh" security-ramboot
	[ -z "$RK_RECOVERY" ] || "$RK_SCRIPTS_DIR/mk-recovery.sh"

	[ -z "$RK_AMP" ] || "$RK_SCRIPTS_DIR/mk-amp.sh"

	"$RK_SCRIPTS_DIR/mk-firmware.sh"

	finish_build
}

build_release()
{
	message "=========================================="
	message "          Start releasing images and build info"
	message "=========================================="

	shift
	RELEASE_BASE_DIR="$RK_OUTDIR/releases/${1:+$1/}${2:-$BOARD}"
	case "$(readlink "$RK_OUTDIR/rootfs")" in
		buildroot) RELEASE_DIR="$RELEASE_BASE_DIR/BUILDROOT" ;;
		debian) RELEASE_DIR="$RELEASE_BASE_DIR/DEBIAN" ;;
		yocto) RELEASE_DIR="$RELEASE_BASE_DIR/YOCTO" ;;
		*) RELEASE_DIR="$RELEASE_BASE_DIR" ;;
	esac
	[ "$1" ] || RELEASE_DIR="$RELEASE_DIR/$(date  +%Y%m%d_%H%M%S)"

	rm -rf "$RELEASE_DIR"
	mkdir -p "$RELEASE_DIR"
	rm -rf "$RELEASE_BASE_DIR/latest"
	ln -rsf "$RELEASE_DIR" "$RELEASE_BASE_DIR/latest"

	message "Saving into $RELEASE_DIR...\n"

	message "Saving images..."
	cp -rvL "$RK_FIRMWARE_DIR" "$RELEASE_DIR/IMAGES"

	if [ "$RK_KERNEL" ]; then
		mkdir -p "$RELEASE_DIR/kernel"

		if [ -d "$RK_OUTDIR/linux-headers" ]; then
			message "Saving linux-headers..."
			cp -rv "$RK_OUTDIR/linux-headers" \
				"$RELEASE_DIR/kernel/"
		fi

		message "Saving kernel files..."
		cp -rv kernel/.config kernel/System.map kernel/vmlinux \
			$RK_KERNEL_DTB "$RELEASE_DIR/kernel/"

		if [ -d "$RK_OUTDIR/kernel-modules" ]; then
			cp -rv $RK_KERNEL_DTB "$RK_OUTDIR/kernel-modules" \
				"$RELEASE_DIR/kernel/"
		fi
	fi

	message "Saving build info..."
	if yes | python3 .repo/repo/repo manifest -r \
		-o "$RELEASE_DIR/manifest.xml"; then
		# Only do this when repositories are available
		message "Saving patches..."
		PATCHES_DIR="$RELEASE_DIR/PATCHES"
		mkdir -p "$PATCHES_DIR"
		.repo/repo/repo forall -j $(( $CPUS + 1 )) -c \
			"\"$RK_SCRIPTS_DIR/release-patches.sh\" \
			\"$PATCHES_DIR/\$REPO_PATH\" \$REPO_PATH \$REPO_LREV"
		install -D -m 0755 "$RK_DATA_DIR/apply-all.sh" "$PATCHES_DIR"
	fi

	message "Saving configs..."
	cp -v "$RK_FINAL_ENV" "$RK_CONFIG" "$RK_DEFCONFIG_LINK" "$RELEASE_DIR/"
	ln -vsf .config "$RELEASE_DIR/build_info"

	message "Saving build logs..."
	cp -rvp "$RK_LOG_BASE_DIR/" "$RELEASE_DIR/"

	rm -rf "$RK_OUTDIR/release"
	ln -vsf "$RELEASE_DIR" "$RK_OUTDIR/release"

	finish_build
}

build_all_release()
{
	message "=========================================="
	message "          Start building and releasing images"
	message "=========================================="

	build_all
	build_release $@

	finish_build
}

# Hooks

usage_hook()
{
	usage_oneline "all" "build images"
	usage_oneline "release[:<subdir>[:<name>]]" \
		"release images and build info"
	usage_oneline "all-release[:<subdir>[:<name>]]" \
		"build and release images"
}

clean_hook()
{
	rm -rf "$RK_OUTDIR" "$RK_OUTDIR"/releases
}

BUILD_CMDS="all all-release"
build_hook()
{
	case "$1" in
		all) build_all ;;
		all-release) build_all_release $@ ;;
	esac
}

POST_BUILD_CMDS="release"
post_build_hook()
{
	build_release $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

case "${1:-all-release}" in
	all) build_all ;;
	all-release) build_all_release $@ ;;
	release) build_release $@ ;;
	*) usage ;;
esac
