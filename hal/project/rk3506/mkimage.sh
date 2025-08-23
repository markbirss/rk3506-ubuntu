#!/bin/bash
# SPDX-License-Identifier: BSD-3-Clause */
# Copyright (c) 2023 Rockchip Electronics Co., Ltd.

set -e

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
	echo "usage: ./mkimage.sh"
}

CUR_DIR=$(pwd)
HAL_DIR=$(pwd)/../..
IMAGE=$(pwd)/Image

$HAL_DIR/tools/mkimage -f $IMAGE/amp_linux.its -E -p 0xe00 $IMAGE/amp.img

echo 'Image: amp.img is ready.'
