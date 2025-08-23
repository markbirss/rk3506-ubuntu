# SPDX-License-Identifier: BSD-3-Clause */

# Copyright (c) 2022 Rockchip Electronics Co., Ltd.

PROJECT		:= RK3506
SOC		:= RK3506
CPU		:= -mcpu=cortex-m0

# BUS M0
CFLAGS := -Os
LDFLAGS := --specs=nosys.specs
LINKER_SCRIPT := gcc_bus_m0.ld

-include ../../../middleware/benchmark/benchmark.mk
-include ../../../middleware/simple_console/simple_console.mk
include ../../common/GCC/Cortex-M.mk
