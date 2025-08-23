#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

message "Adding dirs and links..."

rm -rf mnt/udisk mnt/sdcard mnt/usb_storage  mnt/external_sd udisk sdcard data
mkdir -p mnt/sdcard mnt/udisk
ln -sf udisk mnt/usb_storage
ln -sf sdcard mnt/external_sd
ln -sf mnt/udisk udisk
ln -sf mnt/sdcard sdcard
ln -sf userdata data
