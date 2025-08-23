#!/bin/bash -e

PACKAGE="$1"
COMMAND="${2:-$PACKAGE}"
APT_PACKAGE="${3:-$PACKAGE}"

if ! which "$COMMAND" >/dev/null; then
	echo -e "\e[35m"
	echo "Your $PACKAGE is missing"
	echo "Please install it:"
	echo "sudo apt-get install $APT_PACKAGE"
	echo -e "\e[0m"
	exit 1
fi
