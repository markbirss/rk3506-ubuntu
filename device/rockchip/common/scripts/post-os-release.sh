#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

OS_RELEASE="$TARGET_DIR/etc/os-release"

fixup_os_release()
{
	KEY=$1
	shift

	sed -i "/^$KEY=/d" "$OS_RELEASE"
	echo "$KEY=\"$@\"" >> "$OS_RELEASE"
}

message "Adding information to /etc/os-release..."

mkdir -p "$(dirname "$OS_RELEASE")"
[ -f "$OS_RELEASE" ] || touch "$OS_RELEASE"

BUILD_INFO="$(whoami)@$(hostname) $(date)"
case "$POST_OS" in
	buildroot) BUILD_INFO="$BUILD_INFO - $RK_BUILDROOT_CFG" ;;
	yocto) BUILD_INFO="$BUILD_INFO - ${RK_YOCTO_MACHINE:-$RK_YOCTO_CFG}" ;;
esac

fixup_os_release ID_LIKE "$POST_OS"
fixup_os_release RK_BUILD_INFO "$BUILD_INFO"
