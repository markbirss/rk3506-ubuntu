#!/bin/bash -e

[ -z "$RK_EXTRA_FONTS_DISABLED" ] || exit 0
if [ "$RK_EXTRA_FONTS_DEFAULT" -a "$POST_OS" != yocto ]; then
	notice "No extra fonts for $POST_OS by default"
	exit 0
fi

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"
cd "$OVERLAY_DIR"

EN_FONTS=(dejavu-2.37.tar liberation-2.00.1.tar)
EN_CN_FONTS=(dejavu-2.37.tar liberation-2.00.1.tar source-han-sans-cn-2.004R.tar)

if [ "$RK_EXTRA_FONTS_ENABLED_EN" = "y" ]; then
	for f in "${EN_FONTS[@]}"; do
		message "Installing extra font(${f%.tar}) to $TARGET_DIR..."
		tar xf "$f" -C "$TARGET_DIR"
	done
elif [ "$RK_EXTRA_FONTS_ENABLED_EN_CN" = "y" ]; then
	for f in ${EN_CN_FONTS[@]}; do
		message "Installing extra font(${f%.tar}) to $TARGET_DIR..."
		tar xf "$f" -C "$TARGET_DIR"
	done
else
	message "No font library is installed"
	exit
fi

message "Backup $TARGET_DIR/usr/share/fonts to $TARGET_DIR/usr/lib/fonts"
if [ -d "$TARGET_DIR/usr/lib/fonts" ]; then
	rm -rf "$TARGET_DIR/usr/lib/fonts"
fi
cp -r $TARGET_DIR/usr/share/fonts $TARGET_DIR/usr/lib/fonts
