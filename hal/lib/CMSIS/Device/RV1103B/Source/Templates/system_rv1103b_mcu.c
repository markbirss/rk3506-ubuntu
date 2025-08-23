/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_MCU_CORE

uint32_t SystemCoreClock = 300000000;

extern void clear_bss(void);
/* This part of work usually is done in C library startup code.
 */
static void BssInit(void)
{
    clear_bss();
}

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate(void)
{
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit(void)
{
    /* Do not use uninitializd global variables before BssInit. */
#if defined(HAL_ICACHE_MODULE_ENABLED) || defined(HAL_DCACHE_MODULE_ENABLED)
    uint32_t status;

    /* set the mcu uncache area, usually set the devices address */
    GRF_SYS->CACHE_PERI_ADDR_START = 0x20000;
    GRF_SYS->CACHE_PERI_ADDR_END = 0x21000;
    /* stb enable, stb_entry=7, stb_timeout enable, write back */
    DCACHE->CACHE_CTRL |= DCACHE_CACHE_CTRL_CACHE_EN_MASK |
                          (7U << DCACHE_CACHE_CTRL_CACHE_ENTRY_THRESH_SHIFT) |
                          DCACHE_CACHE_CTRL_STB_TIMEOUT_EN_MASK;
    DCACHE->STB_TIMEOUT_CTRL = 1;

    do {
        status =
            DCACHE->CACHE_STATUS & DCACHE_CACHE_STATUS_CACHE_INIT_FINISH_MASK;
    } while (status == 0);

    DCACHE->CACHE_CTRL &= ~DCACHE_CACHE_CTRL_CACHE_BYPASS_MASK;
#endif
    BssInit();
}

#endif  /* End of HAL_MCU_CORE */
