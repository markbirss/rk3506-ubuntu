#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$(realpath "$RK_SCRIPTS_DIR/../../../..")}"
RK_TOOLS_DIR="${RK_TOOLS_DIR:-$(realpath "$RK_SCRIPTS_DIR/../tools")}"
RK_DEBIAN_ARCH="${RK_DEBIAN_ARCH:-arm64}"
RK_DEBIAN_VERSION="${RK_DEBIAN_VERSION:-bookworm}"

if ! ls $RK_SDK_DIR/debian/ubuntu-build-service/$RK_DEBIAN_VERSION-desktop-$RK_DEBIAN_ARCH >/dev/null 2>&1; then
	echo -e "\e[35m"
	echo "Current SDK doesn't support Debian($RK_DEBIAN_VERSION) for $RK_DEBIAN_ARCH"
	echo "Please try other Debian version."
	echo -e "\e[0m"
	exit 1
fi

if findmnt -fnu -o OPTIONS -T "$RK_SCRIPTS_DIR" | grep -qE "nodev"; then
	echo -e "\e[35m"
	echo "Please remount to allow creating devices on the filesystem:"
	echo "sudo mount -o remount,dev $(findmnt -fnu -o TARGET -T "$RK_SCRIPTS_DIR")"
	echo -e "\e[0m"
	exit 1
fi

# The -d option is required to pack Debian rootfs
if ! mke2fs -h 2>&1 | grep -wq "\-d"; then
	echo -e "\e[35m"
	echo "Your mke2fs is too old: $(mke2fs -V 2>&1 | head -n 1)"
	echo "Please update it:"
	"$RK_SCRIPTS_DIR/install-e2fsprogs.sh"
	echo -e "\e[0m"
	exit 1
fi

if [ ! -e "/usr/share/live/build/data/debian-cd/$RK_DEBIAN_VERSION" ]; then
	echo -e "\e[35m"
	echo "Your live-build doesn't support $RK_DEBIAN_VERSION"
	echo "Please replace it:"
	echo "sudo apt-get remove live-build"
	echo "git clone https://salsa.debian.org/live-team/live-build.git --depth 1 -b debian/1%20230131"
	echo "cd live-build"
	echo "rm -rf manpages/po/"
	echo "sudo make install -j8"
	echo -e "\e[0m"
	exit 1
fi

"$RK_SCRIPTS_DIR/check-package.sh" debootstrap

if [ ! -e "/usr/share/debootstrap/scripts/$RK_DEBIAN_VERSION" ]; then
	echo -e "\e[35m"
	echo "Your debootstrap doesn't support $RK_DEBIAN_VERSION"
	echo "Please replace it:"
	echo "sudo apt-get remove debootstrap"
	echo "git clone https://salsa.debian.org/installer-team/debootstrap.git --depth 1 -b debian/1.0.123+deb11u2"
	echo "cd debootstrap"
	echo "sudo make install -j8"
	echo -e "\e[0m"
	exit 1
fi

case "$RK_DEBIAN_ARCH" in
	arm64) QEMU_ARCH=aarch64 ;;
	armhf) QEMU_ARCH=arm ;;
	*)
		echo -e "\e[35m"
		echo "Unknown arch(RK_DEBIAN_ARCH): $RK_DEBIAN_ARCH!"
		echo -e "\e[0m"
		exit 1 ;;
esac

LDCONFIG="$RK_TOOLS_DIR/../tests/debian/$RK_DEBIAN_VERSION/$QEMU_ARCH/ldconfig"
if [ -e "$LDCONFIG" ] && ! "$LDCONFIG" --version >/dev/null 2>&1; then
	echo -e "\e[35m"
	echo "Your qemu-$QEMU_ARCH doesn't work!"
	echo "Please fix it:"
	echo "sudo apt-get install binfmt-support qemu-user-static --reinstall"
	if [ "$(uname -m)" = x86_64 ]; then
		echo "sudo update-binfmts --disable qemu-$QEMU_ARCH 2>/dev/null"
		echo "sudo update-binfmts --unimport qemu-$QEMU_ARCH 2>/dev/null"
		echo "sudo rm -f /usr/bin/qemu-$QEMU_ARCH-static"
		echo "# Extracted from qemu-user-static_8.0.3+dfsg-4_amd64.deb"
		echo "sudo cp $RK_TOOLS_DIR/x86_64/qemu-$QEMU_ARCH-static /usr/bin/"
		echo "sudo update-binfmts --import qemu-$QEMU_ARCH 2>/dev/null"
		echo "sudo update-binfmts --enable qemu-$QEMU_ARCH 2>/dev/null"
	else
		echo "https://www.qemu.org/download/"
	fi
	echo "# NOTE: If it is running in Docker, do it in the host!"
	echo -e "\e[0m"
	exit 1
fi

# Verify the mirror source
if [ "$RK_DEBIAN_MIRROR" ]; then
	"$RK_SCRIPTS_DIR/check-network.sh" "$RK_DEBIAN_MIRROR" \
		"the Debian mirror source:\n$RK_DEBIAN_MIRROR"
fi
