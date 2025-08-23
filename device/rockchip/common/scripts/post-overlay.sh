#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

RK_RSYNC="rsync -av --chmod=u=rwX,go=rX --copy-unsafe-links --exclude .empty --exclude .git"
RK_OVERLAY_ALLOWED="$@"

[ "$RK_OVERLAY" ] || exit 0

do_install_overlay()
{
	OVERLAY="$(realpath "$1")"
	if [ -x "$OVERLAY/install.sh" ]; then
		notice "Handling overlay: $OVERLAY)..."
		RK_RSYNC="$RK_RSYNC" \
			"$OVERLAY/install.sh" "$TARGET_DIR" "$POST_OS"
	else
		notice "Installing overlay: $OVERLAY to $TARGET_DIR..."
		$RK_RSYNC "$OVERLAY/" "$TARGET_DIR/"
	fi
}

install_overlay()
{
	# For debugging only
	if [ "$RK_OVERLAY_ALLOWED" ]; then
		for d in $RK_OVERLAY_ALLOWED; do
			basename "$d"
		done | grep -wq "$(basename "$1")" || return 0
	fi

	# Install common and chip overlays
	for d in "$RK_COMMON_DIR" "$RK_CHIP_DIR"; do
		OVERLAY="$d/overlays/$1"
		if [ -d "$OVERLAY" ]; then
			do_install_overlay "$OVERLAY"
		fi
	done
}

# Install overlays for recovery, etc.
if [ -z "$POST_ROOTFS" ]; then
	install_overlay $POST_OS
	exit 0
fi

# No overlays for rootfs without RK_ROOTFS_OVERLAY
[ "$RK_ROOTFS_OVERLAY" ] || exit 0

# Install basic rootfs overlays
for d in "$RK_COMMON_DIR" "$RK_CHIP_DIR"; do
	[ -d "$d/overlays/rootfs" ] || continue
	for overlay in $(ls "$d/overlays/rootfs/"); do
		install_overlay "rootfs/$overlay"
	done
done

# Install OS-specific overlays
install_overlay $POST_OS

# Install extra rootfs overlays
for overlay in $RK_ROOTFS_EXTRA_OVERLAY_DIRS; do
	install_overlay "$overlay"
done
