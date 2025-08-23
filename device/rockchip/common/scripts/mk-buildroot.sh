#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"

BUILDROOT_BOARD=$1
ROOTFS_OUTPUT_DIR="${2:-$RK_SDK_DIR/output/buildroot}"
BUILDROOT_DIR="$RK_SDK_DIR/buildroot"

"$RK_SCRIPTS_DIR/check-buildroot.sh"

BUILDROOT_OUTPUT_DIR="$BUILDROOT_DIR/output/$BUILDROOT_BOARD"
BUILDROOT_CONFIG="$BUILDROOT_OUTPUT_DIR/.config"
BUILDROOT_CONFIG_ORIG="$BUILDROOT_OUTPUT_DIR/.config.orig"

# Handle buildroot make
if [ "$2" = make ]; then
	shift
	shift
	if [ ! -r "$BUILDROOT_CONFIG" ]; then
		make -C "$BUILDROOT_DIR" O="$BUILDROOT_OUTPUT_DIR" \
			${BUILDROOT_BOARD}_defconfig
	fi

	case "$1" in
		external/*)
			TARGET=$1
			unset ARG

			COUNT=$(echo $1 | grep -o '-' | wc -l)
			for I in $(seq 1 $COUNT); do
				TARGET=$(echo $1 | cut -d'-' -f1-$I)
				ARG=$(echo $1 | cut -d'-' -f$(($I + 1))-)

				[ -d "$TARGET" ] || continue
				break
			done

			PKG="$(basename $(dirname \
				$(grep -rwl "$TARGET" \
				"$BUILDROOT_DIR/package")))"
			TARGET="$PKG-${ARG:-recreate}"
			;;
		*) TARGET="$1" ;;
	esac

	if [ "$1" ]; then
		shift
		message "Buildroot make: $TARGET $@"
	fi

	make -C "$BUILDROOT_DIR" O="$BUILDROOT_OUTPUT_DIR" $TARGET $@
	exit
fi

# Save the original .config if exists
if [ -r "$BUILDROOT_CONFIG" ] && [ ! -r "$BUILDROOT_CONFIG_ORIG" ]; then
	cp "$BUILDROOT_CONFIG" "$BUILDROOT_CONFIG_ORIG"
fi

make -C "$BUILDROOT_DIR" O="$BUILDROOT_OUTPUT_DIR" ${BUILDROOT_BOARD}_defconfig

# Warn about config changes
if [ -r "$BUILDROOT_CONFIG_ORIG" ]; then
	if ! diff "$BUILDROOT_CONFIG" "$BUILDROOT_CONFIG_ORIG"; then
		warning "Buildroot config changed!"
		warning "You might need to clean it before building:"
		warning "rm -rf $BUILDROOT_OUTPUT_DIR\n"
	fi
fi

# Use buildroot images dir as image output dir
IMAGE_DIR="$BUILDROOT_OUTPUT_DIR/images"
rm -rf "$ROOTFS_OUTPUT_DIR"
mkdir -p "$IMAGE_DIR" "$(dirname "$ROOTFS_OUTPUT_DIR")"
ln -rsf "$IMAGE_DIR" "$ROOTFS_OUTPUT_DIR"
cd "${RK_LOG_DIR:-$ROOTFS_OUTPUT_DIR}"

LOG_PREFIX="br-$(basename "$BUILDROOT_OUTPUT_DIR")"
LOG_FILE="$(start_log "$LOG_PREFIX" 2>/dev/null || echo $PWD/$LOG_PREFIX.log)"
ln -rsf "$LOG_FILE" br.log

case "$BUILDROOT_BOARD" in
	*_recovery) ln -rsf "$LOG_FILE" br-recovery.log ;;
	*_ramboot)
		ln -rsf "$LOG_FILE" br-ramboot.log
		"$RK_SCRIPTS_DIR/check-security.sh" ramboot
		;;
	*)
		ln -rsf "$LOG_FILE" br-rootfs.log
		"$RK_SCRIPTS_DIR/check-security.sh" system
		;;
esac

# Buildroot doesn't like it
unset LD_LIBRARY_PATH

touch "$BUILDROOT_OUTPUT_DIR/.stamp_build_start"
if ! "$BUILDROOT_DIR"/utils/brmake -C "$BUILDROOT_DIR" O="$BUILDROOT_OUTPUT_DIR"; then
	error "Failed to build $BUILDROOT_BOARD:"
	tail -n 100 "$LOG_FILE"
	error "Please check details in $LOG_FILE"
	exit 1
fi
touch "$BUILDROOT_OUTPUT_DIR/.stamp_build_finish"

notice "Log saved on $LOG_FILE"
notice "Generated images:"
ls "$ROOTFS_OUTPUT_DIR"/rootfs.*
