#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$(realpath "$RK_SCRIPTS_DIR/../../../..")}"

# Needs VPN to fetch sources
"$RK_SCRIPTS_DIR/check-network.sh" www.google.com www.google.com \
	"Please setup a VPN to bypass the GFW."

"$RK_SCRIPTS_DIR/check-package.sh" zstd

PYTHON3_MIN_VER=$(python3 --version | cut -d'.' -f2 2>/dev/null)
if [ "${PYTHON3_MIN_VER:-0}" -lt 6 ]; then
	echo -e "\e[35m"
	echo "Your python3 is too old for yocto: $(python3 --version)"
	echo "Please update it:"
	"$RK_SCRIPTS_DIR/install-python3.sh"
	echo -e "\e[0m"
	exit 1
fi

if ! [ "$(git config --global gc.autoDetach)" = false ]; then
	echo -e "\e[35m"
	echo "Please disable the auto-detaching feature of git gc:"
	echo "git config --global gc.autoDetach false"
	echo -e "\e[0m"
	exit 1
fi

for f in "$RK_SDK_DIR"/*/.git/gc.pid "$RK_SDK_DIR"/*/*/.git/gc.pid \
	"$RK_SDK_DIR"/external/*/.git/gc.pid; do
	[ -f "$f" ] || continue
	PROJ="$(dirname "$(dirname "$f")")"

	echo -e "\e[35m"
	echo "GIT is automatically packing loose objects in $PROJ/"
	echo "Waiting for it..."
	while [ -f "$f" ]; do sleep 1; done
	echo -e "\e[0m"
done
