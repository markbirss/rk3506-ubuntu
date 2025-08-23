#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"

# For packing linux-headers .deb package
"$RK_SCRIPTS_DIR/check-package.sh" dpkg
