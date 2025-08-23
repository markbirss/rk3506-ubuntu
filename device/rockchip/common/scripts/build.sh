#!/bin/bash

if [ -z "$BASH_SOURCE" ]; then
	echo "Not in bash, switching to it..."
	case "${@:-shell}" in
		shell) ./build.sh shell ;;
		*)
			./build.sh $@
			bash
			;;
	esac
fi

usage_clean()
{
	usage_oneline "cleanall" "cleanup all"
	for s in $(grep -wl clean_hook "$RK_SCRIPTS_DIR"/mk-*.sh | \
		sed "s/^.*mk-\(.*\).sh/\1/" | grep -v "^all$"); do
		usage_oneline "clean-$s" "cleanup $s"
	done
}

usage()
{
	echo "Usage: $(basename $BASH_SOURCE) [OPTIONS]"
	echo "Available options:"

	run_build_hooks usage

	# Global options
	usage_clean
	usage_oneline "post-rootfs <rootfs dir>" "trigger post-rootfs hook scripts"
	usage_oneline "help" "display this information"
	echo ""
	echo "Default option is '$RK_DEFAULT_TARGET'."

	rm -f "$INITIAL_ENV"
	exit 0
}

# Export global functions
set -a

rk_log()
{
	LOG_COLOR="$1"
	shift
	if [ "$1" = "-n" ]; then
		shift
		LOG_FLAG="-ne"
	else
		LOG_FLAG="-e"
	fi
	echo $LOG_FLAG "\e[${LOG_COLOR}m$@\e[0m"
}

message()
{
	rk_log 36 "$@" # light blue
}

notice()
{
	rk_log 35 "$@" # purple
}

warning()
{
	rk_log 34 "$@" # dark blue
}

error()
{
	rk_log 91 "$@" # light red
}

fatal()
{
	rk_log 31 "$@" # dark red
}

usage_oneline()
{
	printf "%-40s%s\n" "$1" "${*:2}"
}

usage_makefile_oneline()
{
	printf "  %-22s - %s\n" "$(echo "$1" | grep -o "^[^[^:^ ]*")" "${*:2}"
}

finish_build()
{
	notice "Running $(basename "${BASH_SOURCE[1]}") - ${@:-${FUNCNAME[1]}} succeeded."
	cd "$RK_SDK_DIR"
}

load_config()
{
	[ -r "$RK_CONFIG" ] || return 0

	for var in $@; do
		export "$(grep "^$var=" "$RK_CONFIG" | tr -d '"' || true)" \
			&>/dev/null || true
	done
}

check_config()
{
	unset missing
	for var in $@; do
		eval [ -z \"\$$var\" ] || continue

		missing="$missing $var"
	done

	[ "$missing" ] || return 0

	notice "Skipping $(basename "${BASH_SOURCE[1]}") - ${FUNCNAME[1]} for missing configs: $missing."
	return 1
}

kernel_version_raw()
{
	[ -d kernel ] || return 0

	VERSION_KEYS="VERSION PATCHLEVEL"
	VERSION=""

	for k in $VERSION_KEYS; do
		v=$(grep "^$k = " kernel/Makefile | cut -d' ' -f3)
		VERSION=${VERSION:+${VERSION}.}$v
	done
	echo $VERSION
}

kernel_version()
{
	[ -d kernel ] || return 0

	KERNEL_DIR="$(basename "$(realpath kernel)")"
	case "$KERNEL_DIR" in
		kernel-*)
			echo ${KERNEL_DIR#kernel-}
			return 0
			;;
	esac

	kernel_version_raw
}

start_log()
{
	LOG_FILE="$RK_LOG_DIR/${2:-$1_$(date +%F_%H-%M-%S)}.log"
	ln -rsf "$LOG_FILE" "$RK_LOG_DIR/$1.log"
	echo "# $(date +"%F %T")" >> "$LOG_FILE"
	echo "$LOG_FILE"
}

get_toolchain()
{
	MODULE="$1"
	TC_ARCH="${2/arm64/aarch64}"
	TC_VENDOR="${3-none}"
	TC_OS="${4:-linux}"

	MACHINE=$(uname -m)
	if [ "$MACHINE" != x86_64 ]; then
		notice "Using Non-x86 toolchain for $MODULE!" >&2

		if [ "$TC_ARCH" = aarch64 -a "$MACHINE" != aarch64 ]; then
			echo aarch64-linux-gnu-
		elif [ "$TC_ARCH" = arm -a "$MACHINE" != armv7l ]; then
			echo arm-linux-gnueabihf-
		fi
		return 0
	fi

	# RV1126 uses custom toolchain
	if [ "$RK_CHIP_FAMILY" = "rv1126_rv1109" ]; then
		TC_VENDOR=rockchip830
	fi

	TC_DIR="$RK_SDK_DIR/prebuilts/gcc/linux-x86/$TC_ARCH"
	if [ "$TC_VENDOR" ]; then
		TC_PATTERN="$TC_ARCH-$TC_VENDOR-$TC_OS-[^-]*-gcc"
	else
		TC_PATTERN="$TC_ARCH-$TC_OS-[^-]*-gcc"
	fi
	GCC="$(find "$TC_DIR" -name "*gcc" | grep -m 1 "/$TC_PATTERN$" || true)"
	if [ ! -x "$GCC" ]; then
		{
			error "No prebuilt GCC toolchain for $MODULE!"
			error "Arch: $TC_ARCH"
			error "Vendor: $TC_VENDOR"
			error "OS: $TC_OS"
		} >&2
		exit 1
	fi

	echo ${GCC%gcc}
}

ensure_tools()
{
	for t in "$@"; do
		if [ "$RK_ROOTFS_PREFER_PREBUILT_TOOLS" ] || \
			[ "$RK_ROOTFS_PREBUILT_TOOLS" ] || \
			[ ! -e "$t" ]; then
			install -v -D -m 0755 "$RK_TOOLS_DIR/armhf/${t##*/}" "$t"
			continue
		fi

		if [ ! -e "$t" ]; then
			warning "Unable to install $t!"
		fi
	done
}

# For developing shell only

rroot()
{
	cd "$RK_SDK_DIR"
}

rout()
{
	cd "$RK_OUTDIR"
}

rcommon()
{
	cd "$RK_COMMON_DIR"
}

rscript()
{
	cd "$RK_SCRIPTS_DIR"
}

rchip()
{
	cd "$(realpath "$RK_CHIP_DIR")"
}

set +a
# End of global functions

err_handler()
{
	ret=${1:-$?}
	if [ "$ret" -eq 0 ]; then
		return 0
	fi

	fatal "ERROR: Running $BASH_SOURCE - ${2:-${FUNCNAME[1]}} failed!"
	fatal "ERROR: exit code $ret from line ${BASH_LINENO[0]}:"
	fatal "    ${3:-$BASH_COMMAND}"
	fatal "ERROR: call stack:"
	for i in $(seq 1 $((${#FUNCNAME[@]} - 1))); do
		SOURCE="${BASH_SOURCE[$i]}"
		LINE=${BASH_LINENO[$(( $i - 1 ))]}
		fatal "    $(basename "$SOURCE"): ${FUNCNAME[$i]}($LINE)"
	done
	exit $ret
}

# option_check "<supported commands>" <option 1> [option 2] ...
option_check()
{
	CMDS="$1"
	shift

	for opt in $@; do
		for cmd in $CMDS; do
			# NOTE: There might be patterns in commands
			if echo "${opt%%:*}" | grep -qE "^$cmd$"; then
				return 0
			fi
		done
	done
	return 1
}

# hook_check <hook> <stage> <cmd>
hook_check()
{
	case "$2" in
		init | pre-build | build | post-build) ;;
		*) return 0 ;;
	esac

	SCRIPT="$(realpath "$1" --relative-to "$RK_SDK_DIR")"
	CMDS="$(sed -n \
		"s@^RK_${2//-/_}_CMDS[^ ]*\(.*\)\" # $SCRIPT\$@\1@ip" \
		"$RK_PARSED_CMDS")"

	if echo "$CMDS" | grep -wq default; then
		return 0
	fi

	option_check "$CMDS" "$3"
}

# Run specific hook scripts
run_hooks()
{
	DIR="$1"
	shift

	# Prefer chips' hooks than the common ones
	for dir in "$RK_CHIP_DIR/$(basename "$DIR")/" "$DIR"; do
		[ -d "$dir" ] || continue

		for hook in $(find "$dir" -maxdepth 1 -name "*.sh" | sort); do
			# Ignore unrelated hooks
			hook_check "$hook" "$1" "$2" || continue

			if ! "$hook" $@; then
				HOOK_RET=$?
				err_handler $HOOK_RET \
					"${FUNCNAME[0]} $*" "$hook $*"
				exit $HOOK_RET
			fi
		done
	done
}

# Run build hook scripts for normal stages
run_build_hooks()
{
	# Don't log these stages (either interactive or with useless logs)
	case "$1" in
		init | pre-build | make-* | usage | parse-cmds)
			run_hooks "$RK_BUILD_HOOK_DIR" $@ || true
			return 0
			;;
	esac

	LOG_FILE="$(start_log "$1")"

	echo -e "# run hook: $@\n" >> "$LOG_FILE"
	run_hooks "$RK_BUILD_HOOK_DIR" $@ 2>&1 | tee -a "$LOG_FILE"
	HOOK_RET=${PIPESTATUS[0]}
	if [ $HOOK_RET -ne 0 ]; then
		err_handler $HOOK_RET "${FUNCNAME[0]} $*" "$@"
		exit $HOOK_RET
	fi
}

# Run post hook scripts for post-rootfs stage
run_post_hooks()
{
	LOG_FILE="$(start_log post-rootfs)"

	echo -e "# run hook: $@\n" >> "$LOG_FILE"
	run_hooks "$RK_POST_HOOK_DIR" $@ 2>&1 | tee -a "$LOG_FILE"
	HOOK_RET=${PIPESTATUS[0]}
	if [ $HOOK_RET -ne 0 ]; then
		err_handler $HOOK_RET "${FUNCNAME[0]} $*" "$@"
		exit $HOOK_RET
	fi
}

setup_environments()
{
	export LC_ALL=C

	export RK_SCRIPTS_DIR="$(dirname "$(realpath "$BASH_SOURCE")")"
	export RK_COMMON_DIR="$(realpath "$RK_SCRIPTS_DIR/..")"
	export RK_SDK_DIR="$(realpath "$RK_COMMON_DIR/../../..")"
	export RK_DEVICE_DIR="$RK_SDK_DIR/device/rockchip"
	export RK_CHIPS_DIR="$RK_DEVICE_DIR/.chips"
	export RK_CHIP_DIR="$RK_DEVICE_DIR/.chip"

	export RK_DEFAULT_TARGET="all"
	export RK_DATA_DIR="$RK_COMMON_DIR/data"
	export RK_TOOLS_DIR="$RK_COMMON_DIR/tools"
	export RK_EXTRA_PARTS_DIR="$RK_COMMON_DIR/extra-parts"
	export RK_KBUILD_DIR="$RK_COMMON_DIR/linux-kbuild"
	export RK_CONFIG_IN="$RK_COMMON_DIR/configs/Config.in"

	export RK_BUILD_HOOK_DIR="$RK_COMMON_DIR/build-hooks"
	export RK_BUILD_HELPER="$RK_BUILD_HOOK_DIR/build-helper"
	export RK_POST_HOOK_DIR="$RK_COMMON_DIR/post-hooks"
	export RK_POST_HELPER="$RK_POST_HOOK_DIR/post-helper"

	export RK_PARTITION_HELPER="$RK_SCRIPTS_DIR/partition-helper"

	export RK_OUTDIR="$RK_SDK_DIR/output"
	export RK_EXTRA_PART_OUTDIR="$RK_OUTDIR/extra-parts"
	export RK_SESSION_DIR="$RK_OUTDIR/sessions"
	export RK_SESSION="${RK_SESSION:-$(date +%F_%H-%M-%S)}"
	export RK_LOG_DIR="$RK_SESSION_DIR/$RK_SESSION"
	export RK_LOG_BASE_DIR="$RK_OUTDIR/log"
	export RK_ROCKDEV_DIR="$RK_SDK_DIR/rockdev"
	export RK_FIRMWARE_DIR="$RK_OUTDIR/firmware"
	export RK_CONFIG="$RK_OUTDIR/.config"
	export RK_DEFCONFIG_LINK="$RK_OUTDIR/defconfig"
	export RK_OWNER="$(stat --format %U "$RK_SDK_DIR")"
	export RK_OWNER_UID="$(stat --format %u "$RK_SDK_DIR")"

	RK_PARSED_CMDS="$RK_OUTDIR/.parsed_cmds"
	RK_MAKE_USAGE="$RK_OUTDIR/.make_usage"
}

check_sdk() {
	if ! echo "$RK_SCRIPTS_DIR" | \
		grep -q "device/rockchip/common/scripts$"; then
		fatal "SDK corrupted!"
		error "Running $BASH_SOURCE from $RK_SCRIPTS_DIR:"
		ls --file-type "$RK_SCRIPTS_DIR"
		exit 1
	fi

	"$RK_SCRIPTS_DIR/check-sdk.sh"

	# Detect sudo(root)
	unset RK_SUDO_ROOT
	if [ "$RK_OWNER_UID" -ne 0 ] && [ "${USER:-$(id -un)}" = "root" ]; then
		export RK_SUDO_ROOT=1
		notice "Running within sudo(root) environment!"
		echo
	fi
}

parse_scripts()
{
	mkdir -p "$RK_OUTDIR"

	if [ ! -r "$RK_MAKE_USAGE" ] || \
		[ "$(find "$RK_SCRIPTS_DIR" "$RK_CHIP_DIR" "$RK_CHIP_DIR/" \
			-cnewer "$RK_MAKE_USAGE")" ]; then
		{
			TEMP_FILE=$(mktemp -u)

			for c in $(ls "$RK_CHIPS_DIR"); do
				usage_makefile_oneline "$c" "choose $c"
			done > $TEMP_FILE

			run_build_hooks make-usage >> $TEMP_FILE

			usage_clean | \
				while read LINE; do
					usage_makefile_oneline $LINE
				done >> $TEMP_FILE
			mv $TEMP_FILE "$RK_MAKE_USAGE"
		}&
	fi

	if [ ! -r "$RK_PARSED_CMDS" ] || \
		[ "$(find "$RK_SCRIPTS_DIR" "$RK_CHIP_DIR" "$RK_CHIP_DIR/" \
			-cnewer "$RK_PARSED_CMDS")" ]; then
		{
			TEMP_FILE=$(mktemp -u)
			{
				echo "#!/bin/bash"
				run_build_hooks parse-cmds
			} > $TEMP_FILE
			mv $TEMP_FILE "$RK_PARSED_CMDS"
		}&
	fi

	wait
}

makefile_options()
{
	unset DEBUG

	setup_environments
	check_sdk >&2 || exit 1

	parse_scripts

	case "$1" in
		make-usage) cat "$RK_MAKE_USAGE" ;;
		make-targets) cat "$RK_MAKE_USAGE" | awk '{print $1}' ;;
	esac

	exit 0
}

main()
{
	# Early handler of Makefile options
	case "$@" in
		make-*) makefile_options $@ ;;
	esac

	[ -z "$DEBUG" ] || set -x

	trap 'err_handler' ERR
	set -eE

	# Save intial envionments
	unset INITIAL_SESSION
	INITIAL_ENV=$(mktemp -u)
	env > "$INITIAL_ENV"

	[ "$RK_SESSION" ] || INITIAL_SESSION=1

	# Setup basic environments
	setup_environments

	# Log SDK information
	MANIFEST="$RK_SDK_DIR/.repo/manifest.xml"
	if [ -e "$MANIFEST" ]; then
		if [ ! -L "$MANIFEST" ]; then
			MANIFEST="$RK_SDK_DIR/.repo/manifests/$(grep -o "[^\"]*\.xml" "$MANIFEST")"
		fi
		TAG="$(grep -o "linux-.*-gen-rkr[^.\"]*" "$MANIFEST" | \
			head -n 1 || true)"
		MANIFEST="$(basename "$(realpath "$MANIFEST")")"
		notice "\n############### Rockchip Linux SDK ###############\n"
		notice "Manifest: $MANIFEST"
		if [ "$TAG" ]; then
			notice "Version: $TAG"
		fi
		echo
	fi

	notice -n "Log colors: "
	message -n "message "
	notice -n "notice "
	warning -n "warning "
	error -n "error "
	fatal "fatal"
	echo

	# Check SDK requirements
	check_sdk

	# Check for session validation
	if [ -z "$INITIAL_SESSION" ] && [ ! -d "$RK_LOG_DIR" ]; then
		warning "Session($RK_SESSION) is invalid!"

		export RK_SESSION="$(date +%F_%H-%M-%S)"
		export RK_LOG_DIR="$RK_SESSION_DIR/$RK_SESSION"
		INITIAL_SESSION=1
	fi
	export RK_INITIAL_ENV="$RK_LOG_DIR/initial.env"
	export RK_CUSTOM_ENV="$RK_LOG_DIR/custom.env"
	export RK_FINAL_ENV="$RK_LOG_DIR/final.env"

	mkdir -p "$RK_FIRMWARE_DIR"

	cd "$RK_SDK_DIR"
	[ -f README.md ] || ln -rsf "$RK_COMMON_DIR/README.md" .
	[ -d common ] || ln -rsf "$RK_COMMON_DIR" .

	# TODO: Remove it in the repo manifest.xml
	rm -f envsetup.sh

	OPTIONS="${@:-$RK_DEFAULT_TARGET}"

	# Special handle for chip and defconfig
	# e.g. ./build.sh rk3588:rockchip_defconfig
	for opt in $OPTIONS; do
		if [ -d "$RK_CHIPS_DIR/${opt%%:*}" ]; then
			OPTIONS=$(echo "$OPTIONS" | xargs -n 1 | \
				sed "s/^$opt$/chip:$opt/" | xargs)
		elif echo "$opt" | grep -q "^[0-9a-z_]*_defconfig$"; then
			OPTIONS=$(echo "$OPTIONS" | xargs -n 1 | \
				sed "s/^$opt$/defconfig:$opt/" | xargs)
		fi
	done

	# Parse supported commands
	parse_scripts
	source "$RK_PARSED_CMDS"

	# Options checking
	CMDS="$RK_INIT_CMDS $RK_PRE_BUILD_CMDS $RK_BUILD_CMDS \
		$RK_POST_BUILD_CMDS"
	for opt in $OPTIONS; do
		case "$opt" in
			help | h | -h | --help | usage | \?) usage ;;
			clean-*)
				# Check cleanup modules
				for m in $(echo ${opt#clean-} | tr '-' ' '); do
					grep -wq clean_hook \
						"$RK_SCRIPTS_DIR/mk-$m.sh" \
						2>/dev/null || usage
				done
				;&
			shell | buildroot-shell | bshell | cleanall)
				# Check single options
				if [ "$opt" = "$OPTIONS" ]; then
					break
				fi

				error "ERROR: $opt cannot combine with other options!"
				;;
			post-rootfs)
				if [ "$opt" = "$1" -a -d "$2" ]; then
					# Hide args from later checks
					OPTIONS=$opt
					break
				fi

				error "ERROR: $opt should be the first option followed by rootfs dir!"
				;;
			*)
				# Make sure that all options are handled
				if option_check "$CMDS" $opt; then
					continue
				fi

				error "ERROR: Unhandled option: $opt"
				;;
		esac

		usage
	done

	# Prepare log dirs
	if [ ! -d "$RK_LOG_DIR" ]; then
		rm -rf "$RK_LOG_BASE_DIR" "$RK_LOG_DIR" "$RK_SESSION_DIR/latest"
		mkdir -p "$RK_LOG_DIR"
		ln -rsf "$RK_SESSION_DIR" "$RK_LOG_BASE_DIR"
		ln -rsf "$RK_LOG_DIR" "$RK_SESSION_DIR/latest"
		message "Log saved at $RK_LOG_DIR"
	fi

	# Drop old logs
	cd "$RK_LOG_BASE_DIR"
	rm -rf $(ls -t | sed '1,10d')
	cd "$RK_SDK_DIR"

	# Save initial envionments
	if [ "$INITIAL_SESSION" ]; then
		rm -f "$RK_INITIAL_ENV"
		cp "$INITIAL_ENV" "$RK_INITIAL_ENV"
		ln -rsf "$RK_INITIAL_ENV" "$RK_OUTDIR/"
	fi
	rm -f "$INITIAL_ENV"

	# Init stage (preparing SDK configs, etc.)
	run_build_hooks init $OPTIONS
	rm -f "$RK_OUTDIR/.tmpconfig*"

	# No need to go further
	CMDS="$RK_PRE_BUILD_CMDS $RK_BUILD_CMDS $RK_POST_BUILD_CMDS \
		cleanall clean-.* post-rootfs"
	option_check "$CMDS" $OPTIONS || return 0

	# Force exporting config environments
	set -a

	# Load config environments
	source "$RK_CONFIG"
	cp "$RK_CONFIG" "$RK_LOG_DIR"
	export RK_KERNEL_VERSION="$(kernel_version)"

	if [ -z "$INITIAL_SESSION" ]; then
		# Inherit session environments
		sed -n 's/^\(RK_.*=\)\(.*\)/\1"\2"/p' "$RK_FINAL_ENV" > \
			"$INITIAL_ENV"
		source "$INITIAL_ENV"
		rm -f "$INITIAL_ENV"
	else
		# Detect and save custom environments

		# Find custom environments
		rm -f "$RK_CUSTOM_ENV"
		for cfg in $(grep "^RK_" "$RK_INITIAL_ENV" || true); do
			env | grep -q "^${cfg//\"/}$" || \
				echo "$cfg" >> "$RK_CUSTOM_ENV"
		done

		# Allow custom environments overriding
		if [ -e "$RK_CUSTOM_ENV" ]; then
			ln -rsf "$RK_CUSTOM_ENV" "$RK_OUTDIR/"

			warning "WARN: Found custom environments:"
			cat "$RK_CUSTOM_ENV"

			warning "Assuming that is expected, please clear them if otherwise."
			read -t 10 -p "Press enter to continue."
			source "$RK_CUSTOM_ENV"

			if grep -q "^RK_KERNEL_VERSION=" "$RK_CUSTOM_ENV"; then
				warning "Custom RK_KERNEL_VERSION ignored!"
			fi

			if grep -q "^RK_ROOTFS_SYSTEM=" "$RK_CUSTOM_ENV"; then
				warning "Custom RK_ROOTFS_SYSTEM ignored!"
				load_config RK_ROOTFS_SYSTEM
			fi
		fi
	fi

	# Check the Ubuntu image compilation environment
	if [ "$RK_UBUNTU" = "y" ] ;then
		if [ "$(id -u)" -ne 0 ]; then
				error "Please use sudo or run as root to build the Ubuntu image."
				exit 1
		else
				if [ -f $RK_SDK_DIR/kernel/zboot.img ]; then
					sudo chmod a+rw $RK_SDK_DIR/kernel/zboot.img
				fi

				if [ -f $RK_SDK_DIR/u-boot/fit/u-boot.its ]; then
					sudo chmod a+rw $RK_SDK_DIR/u-boot/fit/u-boot.its
				fi

				if [ -f $RK_SDK_DIR/u-boot/fit/uboot.itb ]; then
					sudo chmod a+rw $RK_SDK_DIR/u-boot/fit/uboot.itb
				fi
		fi
	fi

	# Parse partition table
	source "$RK_PARTITION_HELPER"
	rk_partition_init

	set +a

	# The real kernel version: 4.4/4.19/5.10/6.1, etc.
	export RK_KERNEL_VERSION_RAW=$(kernel_version_raw)
	export RK_KERNEL_VERSION="$(kernel_version)"

	# Handle special commands
	case "$OPTIONS" in
		cleanall)
			run_build_hooks clean
			rm -rf "$RK_OUTDIR" "$RK_SDK_DIR/rockdev"
			finish_build cleanall
			exit 0 ;;
		clean-*)
			MODULES="$(echo ${OPTIONS#clean-} | tr '-' ' ')"
			for m in $MODULES; do
				"$RK_SCRIPTS_DIR/mk-$m.sh" clean
			done
			finish_build clean - $MODULES
			exit 0 ;;
		post-rootfs)
			shift
			TARGET_DIR="$1"

			source "$RK_POST_HELPER"
			POST_DIR="$RK_OUTDIR/$POST_OS"
			mkdir -p "$POST_DIR"

			touch "$POST_DIR/.stamp_post_start"
			run_post_hooks "$TARGET_DIR"
			touch "$POST_DIR/.stamp_post_finish"

			ln -rsf "$TARGET_DIR" "$POST_DIR/target"
			finish_build post-rootfs

			notice "Files changed in post-rootfs stage:"
			cd "$TARGET_DIR"
			find . \( -type f -o -type l \) \
				-cnewer "$POST_DIR/.stamp_post_start" | \
				tee "$POST_DIR/.files_post.txt"
			exit 0 ;;
	esac

	# Save final environments
	rm -f "$RK_FINAL_ENV"
	env > "$RK_FINAL_ENV"
	ln -rsf "$RK_FINAL_ENV" "$RK_OUTDIR/"

	# Log configs
	message "\n=========================================="
	message "          Final configs"
	message "=========================================="
	env | grep -E "^RK_.*=.+" | grep -vE "PARTITION_[0-9]" | \
		grep -vE "=\"\"$|_DEFAULT=y|^RK_DEFAULT_TARGET|CMDS=" | \
		grep -vE "^RK_CONFIG|_BASE_CFG=|_LINK=|DIR=|_ENV=|_NAME=|_DTB=" | \
		grep -vE "_HELPER=|_SUPPORTS=|_ARM64=|_ARM=|_HOST=" | \
		grep -vE "^RK_ROOTFS_SYSTEM_|^RK_YOCTO_DISPLAY_PLATFORM_" | sort
	echo

	# Pre-build stage (submodule configuring, etc.)
	run_build_hooks pre-build $OPTIONS

	# Build stage (building, etc.)
	run_build_hooks build $OPTIONS

	# Post-build stage (firmware packing, etc.)
	run_build_hooks post-build $OPTIONS
}

if [ "$0" != "$BASH_SOURCE" ]; then
	# Sourced, executing it directly
	"$BASH_SOURCE" ${@:-shell}
elif [ "$0" == "$BASH_SOURCE" ]; then
	# Executed directly
	main "${@%%:}"
fi
