/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "soc.h"

#if defined(HAL_MCU_CORE) && defined(__RISC_V)

#include "riscv_csr_encoding.h"

# define LREG     lw
# define SREG     sw
# define REGBYTES 4

.globl _start
.weak sc_exit, SystemInit, entry, main
.text
_start:
    la sp, __C_STACK_TOP__

    li a0, 0
    li a1, 0
    call main
    j sc_exit

.globl _wfi
.type _wfi, @function
_wfi:
    wfi
    ret
.globl _fence
.type _fence, @function
_fence:
    fence
    ret

SystemInit:
entry:
    ret

sc_exit:
    j sc_exit

#endif /* defined(HAL_MCU_CORE) && defined(__RISC_V) */
