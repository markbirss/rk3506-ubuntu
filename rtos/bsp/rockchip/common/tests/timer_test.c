/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    timer_test.c
  * @version V1.0
  * @brief   timer test
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-07-01     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdef.h>

#ifdef RT_USING_COMMON_TEST_TIMER

#include "hal_base.h"

static void timer_test_show_usage(void)
{
    rt_kprintf("1. timer_test test timerN\n");
    rt_kprintf("2. timer_test all\n");
    rt_kprintf("like:\n");
    rt_kprintf("\ttimer_test test 0\n");
    rt_kprintf("\ttimer_test all\n");
}

static uint32_t isr_active, time_out;

void timer_isr_helper(uint32_t dev_id);

#ifdef TIMER0
void timer_isr0(void)
{
    timer_isr_helper(0);
}
#endif

#ifdef TIMER1
void timer_isr1(void)
{
    timer_isr_helper(1);
}
#endif

#ifdef TIMER2
void timer_isr2(void)
{
    timer_isr_helper(2);
}
#endif

#ifdef TIMER3
void timer_isr3(void)
{
    timer_isr_helper(3);
}
#endif

#ifdef TIMER4
void timer_isr4(void)
{
    timer_isr_helper(4);
}
#endif

#ifdef TIMER5
void timer_isr5(void)
{
    timer_isr_helper(5);
}
#endif

#ifdef TIMER6
void timer_isr6(void)
{
    timer_isr_helper(6);
}
#endif

#ifdef TIMER7
void timer_isr7(void)
{
    timer_isr_helper(7);
}
#endif

#ifdef TIMER8
void timer_isr8(void)
{
    timer_isr_helper(8);
}
#endif

#ifdef TIMER9
void timer_isr9(void)
{
    timer_isr_helper(9);
}
#endif

#ifdef TIMER10
void timer_isr10(void)
{
    timer_isr_helper(10);
}
#endif

#ifdef TIMER11
void timer_isr11(void)
{
    timer_isr_helper(11);
}
#endif

#ifdef TIMER12
void timer_isr12(void)
{
    timer_isr_helper(12);
}
#endif

#ifdef TIMER13
void timer_isr13(void)
{
    timer_isr_helper(13);
}
#endif

#ifdef TIMER14
void timer_isr14(void)
{
    timer_isr_helper(14);
}
#endif

#ifdef TIMER15
void timer_isr15(void)
{
    timer_isr_helper(15);
}
#endif

#ifdef TIMER16
void timer_isr16(void)
{
    timer_isr_helper(16);
}
#endif

#ifdef TIMER17
void timer_isr17(void)
{
    timer_isr_helper(17);
}
#endif

#ifdef TIMER18
void timer_isr18(void)
{
    timer_isr_helper(18);
}
#endif

#ifdef TIMER19
void timer_isr19(void)
{
    timer_isr_helper(19);
}
#endif

typedef struct _TIMER_DEV
{
    const uint32_t irqNum;
    struct TIMER_REG *pReg;
    void (*isr)(void);

} TIMER_DEV;

#define DEFINE_TIMER_DEV(ID) \
{ \
    .irqNum = TIMER##ID##_IRQn, \
    .pReg =  TIMER##ID, \
    .isr = timer_isr##ID, \
}

static TIMER_DEV s_timer[TIMER_CHAN_CNT] =
{
#ifdef TIMER0
    DEFINE_TIMER_DEV(0),
#endif
#ifdef TIMER1
    DEFINE_TIMER_DEV(1),
#endif
#ifdef TIMER2
    DEFINE_TIMER_DEV(2),
#endif
#ifdef TIMER3
    DEFINE_TIMER_DEV(3),
#endif
#ifdef TIMER4
    DEFINE_TIMER_DEV(4),
#endif
#ifdef TIMER5
    DEFINE_TIMER_DEV(5),
#endif
#ifdef TIMER6
    DEFINE_TIMER_DEV(6),
#endif
#ifdef TIMER7
    DEFINE_TIMER_DEV(7),
#endif
#if TIMER_CHAN_CNT > 8
#ifdef TIMER8
    DEFINE_TIMER_DEV(8),
#endif
#ifdef TIMER9
    DEFINE_TIMER_DEV(9),
#endif
#ifdef TIMER10
    DEFINE_TIMER_DEV(10),
#endif
#ifdef TIMER11
    DEFINE_TIMER_DEV(11),
#endif
#ifdef TIMER12
    DEFINE_TIMER_DEV(12),
#endif
#ifdef TIMER13
    DEFINE_TIMER_DEV(13),
#endif
#ifdef TIMER14
    DEFINE_TIMER_DEV(14),
#endif
#ifdef TIMER15
    DEFINE_TIMER_DEV(15),
#endif
#ifdef TIMER16
    DEFINE_TIMER_DEV(16),
#endif
#ifdef TIMER17
    DEFINE_TIMER_DEV(17),
#endif
#ifdef TIMER18
    DEFINE_TIMER_DEV(18),
#endif
#ifdef TIMER19
    DEFINE_TIMER_DEV(19),
#endif
#endif
};

void timer_isr_helper(uint32_t dev_id)
{
    if (HAL_TIMER_ClrInt(s_timer[dev_id].pReg))
        time_out++;
    isr_active++;
}

static uint64_t timer_get_reload_num(struct TIMER_REG *pReg)
{
    uint64_t loadCount = 0;

    loadCount = (((uint64_t)pReg->LOAD_COUNT[1]) << 32) | (pReg->LOAD_COUNT[0] & 0xffffffff);

    return loadCount;
}

static HAL_Status timer_set_reload_num(struct TIMER_REG *pReg, uint64_t currentVal)
{
    pReg->LOAD_COUNT[0] = (currentVal & 0xffffffff);
    pReg->LOAD_COUNT[1] = ((currentVal >> 32) & 0xffffffff);

    return HAL_OK;
}


void timer_gating_all_enable(void)
{
#if defined(RKMCU_RK2118)
    CRU->GATE_CON[16] = ((0x3ff << 5) << 16) | (0x3ff << 5);
    CRU->GATE_CON[17] = ((0x1f << 0) << 16) | (0x1f << 0);
#else
    RT_ASSERT(0);
#endif
}

void timer_gating_enable(int32_t num)
{
#if defined(RKMCU_RK2118)
    if (num < 8)
    {
        uint32_t bit = 6;

        bit += num;
        if (num > 3)
            bit++;
        CRU->GATE_CON[16] = ((0x1 << bit) << 16) | (0x1 << bit);
    }
    else if (num < 12)
    {
        uint32_t bit = 1;

        bit += num - 8;
        CRU->GATE_CON[17] = ((0x1 << bit) << 16) | (0x1 << bit);
    }
    else
        RT_ASSERT(0);
#else
    RT_ASSERT(0);
#endif
}

void timer_gating_disable(int32_t num)
{
#if defined(RKMCU_RK2118)
    if (num < 8)
    {
        uint32_t bit = 6;

        bit += num;
        if (num > 3)
            bit++;
        CRU->GATE_CON[16] = ((0x1 << bit) << 16) | (0x0 << bit);
    }
    else if (num < 12)
    {
        uint32_t bit = 1;

        bit += num - 8;
        CRU->GATE_CON[17] = ((0x1 << bit) << 16) | (0x0 << bit);
    }
    else
        RT_ASSERT(0);
#else
    RT_ASSERT(0);
#endif
}

void timer_gating_all_disable(void)
{
#if defined(RKMCU_RK2118)
    CRU->GATE_CON[16] = ((0x3ff << 5) << 16) | (0 << 5);
    CRU->GATE_CON[17] = ((0x1f << 0) << 16) | (0 << 0);
#else
    RT_ASSERT(0);
#endif
}

void timer_cru_softrst(void)
{
#if defined(RKMCU_RK2118)
    CRU->SOFTRST_CON[16] = ((0x3ff << 5) << 16) | (0x3ff << 5);
    CRU->SOFTRST_CON[17] = ((0x1f << 0) << 16) | (0x1f << 0);
#else
    RT_ASSERT(0);
#endif
}

void timer_cru_softrst_remove(void)
{
#if defined(RKMCU_RK2118)
    CRU->SOFTRST_CON[16] = ((0x3ff << 5) << 16) | (0 << 5);
    CRU->SOFTRST_CON[17] = ((0x1f << 0) << 16) | (0 << 0);
#else
    RT_ASSERT(0);
#endif
}

int32_t timer_start_stop(struct TIMER_REG *timer_dev, int32_t num)
{
    uint64_t count, count1;

#ifndef IS_FPGA
    timer_gating_disable(num);
    timer_gating_all_disable();
    timer_cru_softrst_remove();
#endif

    /* test timer_dev stop in normalc mode */
    isr_active = 0;
    time_out = 0;
    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, (uint64_t)PLL_INPUT_OSC_RATE); /* Ms count */
    HAL_TIMER_Start(timer_dev);
    count = HAL_TIMER_GetCount(timer_dev);
    HAL_DelayMs(1);
    if (count < HAL_TIMER_GetCount(timer_dev))
        rt_kprintf("[increasing]\n");
    else
        rt_kprintf("[decreasing]\n");
    rt_kprintf("[set finish value]\n");
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count != HAL_TIMER_GetCount(timer_dev)); /* test start*/
    HAL_TIMER_Stop(timer_dev);
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count == HAL_TIMER_GetCount(timer_dev)); /* test stop*/
    rt_kprintf("[support irq mask]\n");
    RT_ASSERT(isr_active == 0 && isr_active == 0);

    /* test timer_dev stop in IT mode */
    isr_active = 0;
    time_out = 0;
    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, HAL_DivU64((uint64_t)PLL_INPUT_OSC_RATE, 1000)); /* Ms count */
    rt_kprintf("1\n");
    HAL_TIMER_Start_IT(timer_dev);
    HAL_DelayMs(10000);
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count != HAL_TIMER_GetCount(timer_dev)); /* test start*/
    HAL_TIMER_Stop_IT(timer_dev);
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count == HAL_TIMER_GetCount(timer_dev)); /* test stop*/
    rt_kprintf("2\n");
    rt_kprintf("[support irq, irq unmask, irq clear]\nisr_active %lu\n", isr_active);
    RT_ASSERT(isr_active >= 10000 && isr_active <= 10050 && time_out == 0);

    rt_kprintf("[set current value]\n");
    count = 0x5a5a5a5aa5a5a5a5;
    timer_set_reload_num(timer_dev, count);
    count1 = timer_get_reload_num(timer_dev);
    RT_ASSERT(count == count1);

    return 0;
}

int32_t timer_cru_test(struct TIMER_REG *timer_dev, int32_t num)
{
#ifndef IS_FPGA
    uint64_t count;

    timer_gating_disable(num);
    timer_gating_all_disable();

    /* test timer_dev stop in normalc mode */
    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, (uint64_t)PLL_INPUT_OSC_RATE);
    HAL_TIMER_Start(timer_dev);

    rt_kprintf("[gate one timer]\n");
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count != HAL_TIMER_GetCount(timer_dev));
    timer_gating_enable(num);
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count == HAL_TIMER_GetCount(timer_dev));
    timer_gating_disable(num);

    rt_kprintf("[gate all timer]\n");
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count != HAL_TIMER_GetCount(timer_dev));
    timer_gating_all_enable();
    count = HAL_TIMER_GetCount(timer_dev);
    RT_ASSERT(count == HAL_TIMER_GetCount(timer_dev));
    timer_gating_all_disable();
    HAL_TIMER_Stop(timer_dev);

    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, (uint64_t)PLL_INPUT_OSC_RATE);
    HAL_TIMER_Start(timer_dev);
    rt_kprintf("[softrst all timer]\n");
    timer_cru_softrst();
    RT_ASSERT(HAL_TIMER_GetCount(timer_dev) == 0);
    timer_cru_softrst_remove();

#ifdef SYS_TIMER
    HAL_TIMER_SysTimerInit(SYS_TIMER);
#endif

#endif

    return 0;
}

int32_t timer_precision_test(struct TIMER_REG *timer_dev)
{
    uint64_t timer_start, timer_end;
    uint32_t systick_start, systick_end;
    int32_t timer_count, systick_count, ideal_count;
    double precision;
    uint32_t systick_clk_source;
    bool dec_timer = false;
    char szBuf[64];
    rt_base_t level;

    /* disable irq */
    level = rt_hw_interrupt_disable();

    /* change systick rate, save clk source, change to processor clock */
    HAL_SYSTICK_Config(SysTick_LOAD_RELOAD_Msk);
    systick_clk_source = SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    /* test timer_dev stop in normalc mode */
    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, (uint64_t)PLL_INPUT_OSC_RATE); /* Ms count */
    HAL_TIMER_Start(timer_dev);

    /* Identify whether the timer is increased or decreased.*/
    timer_start = HAL_TIMER_GetCount(timer_dev);
    if (timer_start < HAL_TIMER_GetCount(timer_dev))
        dec_timer = false;
    else
        dec_timer = true;

    /* test 1ms precision */
    systick_start = SysTick->VAL;
    timer_start = HAL_TIMER_GetCount(timer_dev);
    HAL_DelayMs(1);
    systick_end = SysTick->VAL;
    timer_end = HAL_TIMER_GetCount(timer_dev);
    systick_count = systick_start - systick_end;     /* systick is decreased */
    if (systick_count < 0)                           /* systick overflow */
        systick_count += SysTick->LOAD;
    if (dec_timer)
    {
        timer_count = timer_start - timer_end;
    }
    else
    {
        timer_count = timer_end - timer_start;
    }
    if (timer_count < 0)                            /* timer overflow */
        timer_count += PLL_INPUT_OSC_RATE;
    ideal_count = systick_count / ((SystemCoreClock * 1.0) / PLL_INPUT_OSC_RATE);
    precision = 100.0 - (((timer_count - ideal_count) * 100.0) / ideal_count);
    snprintf(szBuf, sizeof(szBuf), "1ms precision: %f(100 is ideal)", precision);
    //rt_kprintf("systick: %d, %d; timer: %lld, %lld;\n", systick_start, systick_end, timer_start, timer_end);
    rt_kprintf("%s\n", szBuf);

    /* test 10ms precision */
    systick_start = SysTick->VAL;
    timer_start = HAL_TIMER_GetCount(timer_dev);
    HAL_DelayMs(10);
    systick_end = SysTick->VAL;
    timer_end = HAL_TIMER_GetCount(timer_dev);
    systick_count = systick_start - systick_end;     /* systick is decreased */
    if (systick_count < 0)                           /* systick overflow */
        systick_count += SysTick->LOAD;
    if (dec_timer)
    {
        timer_count = timer_start - timer_end;
    }
    else
    {
        timer_count = timer_end - timer_start;
    }
    if (timer_count < 0)                            /* timer overflow */
        timer_count += PLL_INPUT_OSC_RATE;
    ideal_count = systick_count / ((SystemCoreClock * 1.0) / PLL_INPUT_OSC_RATE);
    precision = 100.0 - (((timer_count - ideal_count) * 100.0) / ideal_count);
    snprintf(szBuf, sizeof(szBuf), "10ms precision: %f(100 is ideal)", precision);
    //rt_kprintf("systick: %d, %d; timer: %lld, %lld;\n", systick_start, systick_end, timer_start, timer_end);
    rt_kprintf("%s\n", szBuf);

    /* test 100ms precision */
    systick_start = SysTick->VAL;
    timer_start = HAL_TIMER_GetCount(timer_dev);
    HAL_DelayMs(100);
    systick_end = SysTick->VAL;
    timer_end = HAL_TIMER_GetCount(timer_dev);
    systick_count = systick_start - systick_end;     /* systick is decreased */
    if (systick_count < 0)                           /* systick overflow */
        systick_count += SysTick->LOAD;
    if (dec_timer)
    {
        timer_count = timer_start - timer_end;
    }
    else
    {
        timer_count = timer_end - timer_start;
    }
    if (timer_count < 0)                             /* timer overflow */
        timer_count += PLL_INPUT_OSC_RATE;
    ideal_count = systick_count / ((SystemCoreClock * 1.0) / PLL_INPUT_OSC_RATE);
    precision = 100.0 - (((timer_count - ideal_count) * 100.0) / ideal_count);
    snprintf(szBuf, sizeof(szBuf), "100ms precision: %f(100 is ideal)", precision);
    //rt_kprintf("systick: %d, %d; timer: %lld, %lld;\n", systick_start, systick_end, timer_start, timer_end);
    rt_kprintf("%s\n", szBuf);

    HAL_TIMER_Stop(timer_dev);

    /* resume systick rate & clk source */
    HAL_SYSTICK_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    SysTick->CTRL |= systick_clk_source;

    /* resume irq */
    rt_hw_interrupt_enable(level);

    return 0;
}

int32_t timer_freq_test(struct TIMER_REG *timer_dev, int32_t num)
{
#ifdef CLK_RKTIMER0_TIME0
    uint64_t timer_start, timer_end;
    uint32_t count_24m, count_100m;
    eCLOCK_Name clk = CLK_RKTIMER0_TIME0;
    uint32_t rate;
    float ratio, stand, deviation;
    char szBuf[32];

    switch (num)
    {
    case 0:
        clk = CLK_RKTIMER0_TIME0;
        break;
    case 1:
        clk = CLK_RKTIMER0_TIME1;
        break;
    case 2:
        clk = CLK_RKTIMER0_TIME2;
        break;
    case 3:
        clk = CLK_RKTIMER0_TIME3;
        break;
    case 4:
        clk = CLK_RKTIMER1_TIME0;
        break;
    case 5:
        clk = CLK_RKTIMER1_TIME1;
        break;
    case 6:
        clk = CLK_RKTIMER1_TIME2;
        break;
    case 7:
        clk = CLK_RKTIMER1_TIME3;
        break;
    }

    /* init and start timer */
    HAL_TIMER_Init(timer_dev, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer_dev, (uint64_t) -1);
    HAL_TIMER_Start(timer_dev);

    /* get 10ms timer count with 24M */
    HAL_CRU_ClkSetFreq(clk, 24000000);
    timer_start = HAL_TIMER_GetCount(timer_dev);
    HAL_CPUDelayUs(100000);
    timer_end = HAL_TIMER_GetCount(timer_dev);
    count_24m = timer_end > timer_start ? (timer_end - timer_start) : (timer_start - timer_end);

    /* get 10ms timer count with 100M */
    HAL_CRU_ClkSetFreq(clk, 100000000);
    rate = HAL_CRU_ClkGetFreq(clk);
    if (rate != 100000000)
    {
        rt_kprintf("set timer%d freq fail: %d\n", num, rate);
    }
    timer_start = HAL_TIMER_GetCount(timer_dev);
    HAL_CPUDelayUs(100000);
    timer_end = HAL_TIMER_GetCount(timer_dev);
    count_100m = timer_end > timer_start ? (timer_end - timer_start) : (timer_start - timer_end);
    ratio = (count_100m * 1.0) / count_24m;

    stand = 100.0 / 24;
    deviation = ratio > stand ? (ratio - stand) : (stand - ratio);
    deviation = deviation / stand;
    snprintf(szBuf, sizeof(szBuf), "deviation=%f", deviation);
    rt_kprintf("%s\n", szBuf);
    RT_ASSERT(deviation < 0.01);
    HAL_TIMER_Stop(timer_dev);
#endif

    return 0;
}

static void timer_test_loop(int32_t num)
{
    if (num >= TIMER_CHAN_CNT)
    {
        rt_kprintf("only %d timer\n", TIMER_CHAN_CNT);
        return;
    }

    if (s_timer[num].pReg == SYS_TIMER)
    {
        rt_kprintf("test timer should not equals sys_timer\n");
        return;
    }

    rt_kprintf("TIMER%ld: %s begin\n", num, __func__);

    rt_hw_interrupt_install(s_timer[num].irqNum, (void *)s_timer[num].isr, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(s_timer[num].irqNum);

    if (timer_precision_test(s_timer[num].pReg) == 0)
        rt_kprintf("TIMER%ld: precision pass\n\n", num);

#ifdef RKMCU_RK2118
    /* timer12-timer19 need scru, just ignore */
    if (num >= 12)
        return;
#endif

    if (timer_start_stop(s_timer[num].pReg, num) == 0)
        rt_kprintf("TIMER%ld: function pass\n", num);

    if (timer_cru_test(s_timer[num].pReg, num) == 0)
        rt_kprintf("TIMER%ld: cru pass\n\n", num);

    if (timer_freq_test(s_timer[num].pReg, num) == 0)
        rt_kprintf("TIMER%ld: freq pass\n\n", num);
}

void hw_timer_test(int32_t argc, char **argv)
{
    char *cmd;

    if (argc < 2)
        goto out;

    cmd = argv[1];
    if (!rt_strcmp(cmd, "test"))
    {
        int32_t num;

        if (argc < 3)
            goto out;

        num = atoi(argv[2]);
        timer_test_loop(num);
    }
    else if (!rt_strcmp(cmd, "all"))
    {
        int32_t i;

        for (i = 0; i < HAL_ARRAY_SIZE(s_timer); i++)
        {
            if (s_timer[i].pReg == SYS_TIMER)
                continue;
            timer_test_loop(i);
        }
    }
    else
    {
        goto out;
    }

    return;
out:
    timer_test_show_usage();
    return;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(hw_timer_test, timer test cmd);
#endif

#endif
