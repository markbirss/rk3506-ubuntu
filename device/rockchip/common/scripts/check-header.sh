#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"

PACKAGE="$1"
HEADER="$2"
APT_PACKAGE="$3"

if echo | gcc -E -include "$HEADER" - &>/dev/null; then
	exit 0
fi

"$RK_SCRIPTS_DIR/check-package.sh" "$PACKAGE header" should-not-exists "$APT_PACKAGE"
