# SPDX-License-Identifier: BSD-3-Clause */

# Copyright (c) 2023 Rockchip Electronics Co., Ltd.

PROJECT		:= RK3576
SOC		:= RK3576
CPU		:= -mcpu=cortex-m0

# BUS M0
CFLAGS := -O2
LDFLAGS := --specs=nosys.specs
LINKER_SCRIPT := gcc_bus_m0.ld

# PMU M0
# CFLAGS := -Os
# LDFLAGS := --specs=nano.specs
# LINKER_SCRIPT := gcc_pmu_m0.ld

-include ../../../middleware/benchmark/benchmark.mk
include ../../common/GCC/Cortex-M.mk
