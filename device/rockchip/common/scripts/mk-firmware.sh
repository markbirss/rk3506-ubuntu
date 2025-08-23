#!/bin/bash -e

link_image() {
	SRC="$1"
	DST="$2"
	message "Linking $DST from $SRC..."
	ln -rsf "$SRC" "$DST"
}

build_firmware()
{
	mkdir -p "$RK_FIRMWARE_DIR"

	# Legacy rockdev/
	rm -rf "$RK_ROCKDEV_DIR"
	ln -rsf "$RK_FIRMWARE_DIR" "$RK_ROCKDEV_DIR"

	"$RK_SCRIPTS_DIR/check-grow-align.sh"

	link_image "$RK_CHIP_DIR/$RK_PARAMETER" "$RK_FIRMWARE_DIR/parameter.txt"

	"$RK_SCRIPTS_DIR/mk-extra-parts.sh"

	# Make sure that the loader is ready
	if [ ! -r "$RK_FIRMWARE_DIR/MiniLoaderAll.bin" ]; then
		notice "Loader is not ready, building it..."
		"$RK_SCRIPTS_DIR/mk-loader.sh"
	fi

	notice "Packed files:"
	for f in "$RK_FIRMWARE_DIR"/*; do
		NAME=$(basename "$f")

		if [ ! -r "$f" ]; then
			warning "$NAME($(readlink -f "$f")) is invalid!"
			continue
		fi

		echo -n "$NAME"
		if [ -L "$f" ]; then
			echo -n "($(readlink -f "$f"))"
		fi

		FILE_SIZE=$(ls -lLh $f | xargs | cut -d' ' -f 5)
		echo ": $FILE_SIZE"

		echo "$NAME" | grep -q ".img$" || continue

		# Assert the image's size smaller then the limit
		PART_NAME="${NAME%.img}"
		PART_SIZE_KB="$(rk_partition_size_kb "$PART_NAME")"

		if [ "$PART_NAME" = rootfs -a "$PART_SIZE_KB" -eq 0 ]; then
			PART_NAME=system
			PART_SIZE_KB="$(rk_partition_size_kb "$PART_NAME")"
		fi

		[ ! "$PART_SIZE_KB" -eq 0 ] || continue

		FILE_SIZE_KB="$(( $(stat -Lc "%s" "$f") / 1024 ))"
		if [ "$PART_SIZE_KB" -lt "$FILE_SIZE_KB" ]; then
			error "error: $NAME's size exceed parameter's $PART_NAME partition size limit!"
			return 1
		fi
	done

	[ -z "$RK_UPDATE" ] || "$RK_SCRIPTS_DIR/mk-updateimg.sh"

	message "Images under $RK_FIRMWARE_DIR/ are ready!"

	finish_build
}

# Hooks

usage_hook()
{
	usage_oneline "firmware" "pack and check firmwares"
}

clean_hook()
{
	rm -rf "$RK_FIRMWARE_DIR" "$RK_ROCKDEV_DIR"
}

POST_BUILD_CMDS="firmware"
post_build_hook()
{
	message "=========================================="
	message "          Start packing firmwares"
	message "=========================================="

	build_firmware
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

post_build_hook $@
