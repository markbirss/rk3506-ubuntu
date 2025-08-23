#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"

if ! which python2 >/dev/null; then
	echo -e "\e[35m"
	echo "Your python2 is missing for U-Boot"
	echo "Please install it:"
	"$RK_SCRIPTS_DIR/install-python2.sh"
	echo -e "\e[0m"
	exit 1
fi
