#!/bin/bash -e

# Hooks

usage_hook()
{
	usage_oneline "extra-parts" "pack extra partition images"
}

clean_hook()
{
	rm -rf "$RK_EXTRA_PART_OUTDIR"

	for idx in $(seq 1 "$(rk_extra_part_num)"); do
		IMAGE="$(basename "$(rk_extra_part_img $idx)")"
		rm -rf "$RK_FIRMWARE_DIR/$IMAGE"
	done
}

POST_BUILD_CMDS="extra-parts"
post_build_hook()
{
	"$RK_SCRIPTS_DIR/check-package.sh" fakeroot

	for idx in $(seq 1 "$(rk_extra_part_num)"); do
		PART_NAME="$(rk_extra_part_name $idx)"
		FS_TYPE="$(rk_extra_part_fstype $idx)"
		SIZE="$(rk_extra_part_size $idx)"
		FAKEROOT_SCRIPT="$(rk_extra_part_fakeroot_script $idx)"
		OUTDIR="$(rk_extra_part_outdir $idx)"
		DST="$(rk_extra_part_img $idx)"

		rk_extra_part_prepare $idx

		if rk_extra_part_builtin $idx; then
			notice "Skip packing $PART_NAME (builtin)"
			continue
		fi

		if [ "$SIZE" = max ]; then
			SIZE="$(rk_partition_size_kb "$PART_NAME")K"
			if [ "$SIZE" = 0K ]; then
				if [ "$FS_TYPE" != ubi ]; then
					error "Unable to detect max size of $PART_NAME"
					return 1
				fi

				SIZE="${RK_FLASH_SIZE}M"
				notice "Flash storage size is $SIZE"
			fi

			notice "Using maxium size($SIZE) for $PART_NAME"
		fi

		sed -i '/mk-image.sh/d' "$FAKEROOT_SCRIPT"
		echo "\"$RK_SCRIPTS_DIR/mk-image.sh\" \
			-t \"$FS_TYPE\" -s \"$SIZE\" -l \"$PART_NAME\" \
			\"$OUTDIR\" \"$DST\"" >> "$FAKEROOT_SCRIPT"

		notice "Packing $DST from $FAKEROOT_SCRIPT"
		cd "$OUTDIR"
		fakeroot -- "$FAKEROOT_SCRIPT"
		notice "Done packing $DST"

		ln -rsf "$DST" "$RK_FIRMWARE_DIR/"

		if ! rk_partition_parse_names | grep -qE "\<$PART_NAME\>"; then
			warning "Packed $DST without having $PART_NAME partition!"
		fi
	done

	finish_build build_extra_part
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

post_build_hook
