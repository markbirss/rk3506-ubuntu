#!/bin/sh

../tools/upgrade_tool db Image/rk2106_db_loader.bin
../tools/upgrade_tool wl 0 Image/Firmware.img
../tools/upgrade_tool rd
