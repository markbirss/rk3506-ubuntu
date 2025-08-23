# SPDX-License-Identifier: BSD-3-Clause */

# Copyright (c) 2023 Rockchip Electronics Co., Ltd.

PROJECT		:= RV1106
SOC		:= RV1106
CPU		:= -mcpu=cortex-a7 -mfloat-abi=hard

FIRMWARE_CPU_BASE ?= 0x00200000
DRAM_SIZE ?= 0x00800000

CFLAGS += -DFIRMWARE_BASE=$(FIRMWARE_CPU_BASE) -DDRAM_SIZE=$(DRAM_SIZE)
CFLAGS += -DSRAM_BASE=$(SRAM_BASE) -DSRAM_SIZE=$(SRAM_SIZE)

include ../../common/GCC/Cortex-A.mk
