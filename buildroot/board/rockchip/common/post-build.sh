#!/bin/bash -e

TARGET_DIR="${TARGET_DIR:-"$@"}"

# Export configs to environment
export $(grep -E "^BR2_.*=y|^BR2_DEFCONFIG=" \
	"${BR2_CONFIG:-"$TARGET_DIR/../.config"}")

OVERLAYS="$(dirname "$0")/overlays"
for dir in $(ls "$OVERLAYS"); do
	OVERLAY_DIR="$OVERLAYS/$dir"
	if [ -x "$OVERLAY_DIR/prepare.sh" ] && \
		! "$OVERLAY_DIR/prepare.sh" "$TARGET_DIR"; then
		echo ">>> Ignored $OVERLAY_DIR"
		continue
	fi

	echo ">>> Copying $OVERLAY_DIR"
	rsync -av --chmod=u=rwX,go=rX --exclude .empty --exclude /prepare.sh \
		"$OVERLAY_DIR/" "$TARGET_DIR/"
done

if [ -z "$RK_SESSION" ]; then
        echo -e "\e[35m>>> Building buildroot directly for Rockchip is dangerous!\e[0m"
fi

POST_SCRIPT="../device/rockchip/common/post-build.sh"
[ -x "$POST_SCRIPT" ] || exit 0

# Filter out host pathes
export PATH="$(echo $PATH | xargs -d':' -n 1 | grep -v "^$O" | paste -sd':')"

$POST_SCRIPT "$(realpath "$TARGET_DIR")" "$(basename "$BR2_DEFCONFIG")" 2>&1 | \
	while read line; do
		if echo "$line" | \
			grep -iqE "building|running|handling|installing"; then
			echo -n ">>> "
		fi
		echo -e "$line"
	done
