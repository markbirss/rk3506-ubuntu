#!/bin/sh

upgrade_tool ul Bin/RV1108_usb_boot_V1.26.bin
upgrade_tool wl 0x40 Bin/Firmware.img
upgrade_tool wl 0x1000 ./rtthread.bin
upgrade_tool rd
