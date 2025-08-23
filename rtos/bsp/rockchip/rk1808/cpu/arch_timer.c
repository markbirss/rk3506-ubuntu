/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-01     Frank Wang   the first version
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <time.h>

#include <rthw.h>
#include <rtthread.h>
#include <rtdebug.h>

#include <interrupt.h>
#include <soc.h>
#include "hal_base.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define INT_MAX     2147483647

/* Arch Generic timer PPI irq number */

#define ARCH_TIMER_PPI_IRQ  NoSecurePhyTimer_IRQn
#define ARCH_STIME_BASE     STIMER_BASE
#define COUNTER_FREQUENCY   24000000

/* CNTFRQ AArch32 register */

#define TIMER_REG_CNTFRQ    "c0, 0"

/* CNTP AArch32 registers */

#define TIMER_REG_CNTP_CTL  "c2, 1"
#define TIMER_REG_CNTP_CVAL "2"
#define TIMER_REG_CNTP_TVAL "c2, 0"
#define TIMER_REG_CNTPCT    "0"

/* CNTPS AArch32 registers are banked and accessed though CNTP */

#define CNTPS CNTP

/* CNTV AArch32 registers */

#define TIMER_REG_CNTV_CTL  "c3, 1"
#define TIMER_REG_CNTV_CVAL "3"
#define TIMER_REG_CNTV_TVAL "c3, 0"
#define TIMER_REG_CNTVCT    "1"

#define READ_TIMER_REG32(reg) \
({ \
    uint32_t _val; \
    __asm__ volatile("mrc p15, 0, %0, c14, " reg : "=r" (_val)); \
    _val; \
})

#define READ_TIMER_REG64(reg) \
({ \
    uint64_t _val; \
    __asm__ volatile("mrrc p15, " reg ", %0, %H0, c14" : "=r" (_val)); \
    _val; \
})

#define WRITE_TIMER_REG32(reg, val) \
({ \
    __asm__ volatile("mcr p15, 0, %0, c14, " reg :: "r" (val)); \
    __asm__ volatile("isb" ::: "memory"); \
})

#define WRITE_TIMER_REG64(reg, val) \
({ \
    __asm__ volatile("mcrr p15, " reg ", %0, %H0, c14" :: "r" (val)); \
    __asm__ volatile("isb" ::: "memory"); \
})

#ifndef TIMER_ARM_GENERIC_SELECTED
#define TIMER_ARM_GENERIC_SELECTED CNTP
#endif

#define COMBINE3(a,b,c) a ## b ## c
#define XCOMBINE3(a,b,c) COMBINE3(a, b, c)

#define SELECTED_TIMER_REG(reg) XCOMBINE3(TIMER_REG_, TIMER_ARM_GENERIC_SELECTED, reg)
#define TIMER_REG_CTL       SELECTED_TIMER_REG(_CTL)
#define TIMER_REG_CVAL      SELECTED_TIMER_REG(_CVAL)
#define TIMER_REG_TVAL      SELECTED_TIMER_REG(_TVAL)
#define TIMER_REG_CT        SELECTED_TIMER_REG(CT)

/* low 32 bits */
#define STIMER0_CHN_BASE(n)     (ARCH_STIME_BASE + 0x20 * (n))
#define TIMER_LOAD_COUNT0       0x00
#define TIMER_LOAD_COUNT1       0x04
#define TIMER_CURRENT_VALUE0    0x08
#define TIMER_CURRENT_VALUE1    0x0C
#define TIMER_CONTROL_REG       0x10
#define TIMER_INTSTATUS         0x18

#define TIMER_DIS               0x0
#define TIMER_EN                0x1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct fp_32_64
{
    uint32_t l0;    /* unshifted value */
    uint32_t l32;   /* value shifted left 32 bits (or bit -1 to -32) */
    uint32_t l64;   /* value shifted left 64 bits (or bit -33 to -64) */
};

struct fp_32_64 cntpct_per_ms;
struct fp_32_64 ms_per_cntpct;
struct fp_32_64 us_per_cntpct;

static uint32_t timer_reload_val;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static uint64_t
mul_u32_u32(uint32_t a, uint32_t b, int a_shift, int b_shift)
{
    return (uint64_t)a * b;
}

static uint64_t
u64_mul_u32_fp32_64(uint32_t a, struct fp_32_64 b)
{
    uint64_t tmp;
    uint64_t res_0;
    uint64_t res_l32;
    uint32_t res_l32_32;
    uint64_t ret;

    res_0 = mul_u32_u32(a, b.l0, 0, 0);
    tmp = mul_u32_u32(a, b.l32, 0, -32);
    res_0 += tmp >> 32;
    res_l32 = (uint32_t)tmp;
    res_l32 += mul_u32_u32(a, b.l64, 0, -64) >> 32; /* Improve rounding accuracy */
    res_0 += res_l32 >> 32;
    res_l32_32 = res_l32;
    ret = res_0 + (res_l32_32 >> 31); /* Round to nearest integer */

    return ret;
}

static void
fp_32_64_div_32_32(struct fp_32_64 *result, uint32_t dividend, uint32_t divisor)
{
    uint64_t tmp;
    uint32_t rem;

    tmp = ((uint64_t)dividend << 32) / divisor;
    rem = ((uint64_t)dividend << 32) % divisor;
    result->l0 = tmp >> 32;
    result->l32 = tmp;
    tmp = ((uint64_t)rem << 32) / divisor;
    result->l64 = tmp;
}

static inline uint64_t time_to_cntpct(uint32_t time)
{
    return u64_mul_u32_fp32_64(time, cntpct_per_ms);
}

static inline uint32_t read_cntfrq(void)
{
    return (uint32_t)READ_TIMER_REG32(TIMER_REG_CNTFRQ);
}

#if 0
static uint32_t read_cntp_ctl(void)
{
    uint32_t cntp_ctl;

    cntp_ctl = READ_TIMER_REG32(TIMER_REG_CTL);
    return cntp_ctl;
}
#endif

static inline void write_cntp_ctl(uint32_t cntp_ctl)
{
    WRITE_TIMER_REG32(TIMER_REG_CTL, cntp_ctl);
}

static inline void write_cntp_cval(uint64_t cntp_cval)
{
    WRITE_TIMER_REG64(TIMER_REG_CVAL, cntp_cval);
}

static inline void write_cntp_tval(int32_t cntp_tval)
{
    WRITE_TIMER_REG32(TIMER_REG_TVAL, cntp_tval);
}

static inline uint64_t read_cntpct(void)
{
    return (uint64_t)READ_TIMER_REG64(TIMER_REG_CT);
}

static int platform_set_oneshot_timer(uint32_t time)
{
    uint64_t cntpct_interval = time_to_cntpct(time);

    if (cntpct_interval <= INT_MAX)
        write_cntp_tval(cntpct_interval);
    else
        write_cntp_cval(read_cntpct() + cntpct_interval);

    write_cntp_ctl(1);

    return 0;
}

static void arch_timer_isr(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    write_cntp_ctl(0);

    /* Process timer interrupt */
    HAL_IncTick();
    rt_tick_increase();

    /* Kick next */
    platform_set_oneshot_timer(timer_reload_val); /* 10ms */

    /* leave interrupt */
    rt_interrupt_leave();
}

static void arch_timer_init_conversion_factors(uint32_t cntfrq)
{
    fp_32_64_div_32_32(&cntpct_per_ms, cntfrq, 1000);
    fp_32_64_div_32_32(&ms_per_cntpct, 1000, cntfrq);
    fp_32_64_div_32_32(&us_per_cntpct, 1000 * 1000, cntfrq);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void secure_timer_init(void)
{
    __REG32(STIMER0_CHN_BASE(1) + TIMER_CONTROL_REG) = TIMER_DIS;
    __REG32(STIMER0_CHN_BASE(1) + TIMER_LOAD_COUNT0) = 0xffffffff;
    __REG32(STIMER0_CHN_BASE(1) + TIMER_LOAD_COUNT1) = 0xffffffff;

    /* auto reload & enable the timer */
    __REG32(STIMER0_CHN_BASE(1) + TIMER_CONTROL_REG) = TIMER_EN;
}

int arm_timer_initialize(void)
{
    uint32_t cntfrq;

    secure_timer_init();

    /* Make sure the timer interrupt is disabled */
    rt_hw_interrupt_mask(ARCH_TIMER_PPI_IRQ);

    cntfrq = read_cntfrq();
    if (!cntfrq)
    {
        rt_kprintf("Got timer frequence is 0, use default frequence\n");
        cntfrq = COUNTER_FREQUENCY;
    }

    rt_kprintf("cntfrq: 0x%08x, %u\n", cntfrq, cntfrq);

    arch_timer_init_conversion_factors(cntfrq);

    timer_reload_val = 1000 / RT_TICK_PER_SECOND; /* 10ms */

    /* Attach the timer interrupt vector */
    rt_hw_interrupt_install(ARCH_TIMER_PPI_IRQ, (rt_isr_handler_t)arch_timer_isr,
                            RT_NULL, "tick");

    /* Set oneshort timer */
    platform_set_oneshot_timer(timer_reload_val);

    /* And enable the timer interrupt */
    rt_hw_interrupt_umask(ARCH_TIMER_PPI_IRQ);

    /* System tick init */
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);

    return RT_EOK;
}
INIT_BOARD_EXPORT(arm_timer_initialize);
