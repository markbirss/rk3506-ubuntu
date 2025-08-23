/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"
#include "hal_base.h"
#include "unity_runner.h"
#include <stdlib.h>

/********************* Private MACRO Definition ******************************/
//#define GPIO_VIRTUAL_MODEL_TEST
//#define PERF_TEST
//#define SOFTIRQ_TEST
//#define SYSTICK_TEST
//#define TIMER_TEST
//#define TIMER_INTMUX_TEST

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/************************************************/
/*                                              */
/*                 HW Borad config              */
/*                                              */
/************************************************/

/* TODO: Set Module IOMUX Function Here */

/************************************************/
/*                                              */
/*          GPIO_VIRTUAL_MODEL_TEST             */
/*                                              */
/************************************************/
#ifdef GPIO_VIRTUAL_MODEL_TEST
static void gpio0_exp3_isr(uint32_t irq, void *args)
{
    HAL_GPIO_IRQHandler(GPIO0_EXP3, GPIO_BANK0_EXP3);
}

static HAL_Status c3_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIO0C3 exp callback!\n");

    return HAL_OK;
}

static void gpio_virtual_model_test(void)
{
    uint32_t level;

    /* set pinctrl function */
    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_C3,
                         PIN_CONFIG_MUX_FUNC0);

    /* set pin group configuration */
    HAL_GPIO_EnableVirtualModel(GPIO0);
    HAL_GPIO_SetVirtualModel(GPIO0,
                             GPIO_PIN_A0 | GPIO_PIN_A1 | GPIO_PIN_A2 | GPIO_PIN_A3 |
                             GPIO_PIN_A4 | GPIO_PIN_A5 | GPIO_PIN_A6 | GPIO_PIN_A7 |
                             GPIO_PIN_B0 | GPIO_PIN_B1 | GPIO_PIN_B2 | GPIO_PIN_B3 |
                             GPIO_PIN_B4 | GPIO_PIN_B5 | GPIO_PIN_B6 | GPIO_PIN_B7 |
                             GPIO_PIN_C0 | GPIO_PIN_C1 | GPIO_PIN_C2 |
                             GPIO_PIN_C4 | GPIO_PIN_C5 | GPIO_PIN_C6 | GPIO_PIN_C7 |
                             GPIO_PIN_D0 | GPIO_PIN_D1 | GPIO_PIN_D2 | GPIO_PIN_D3 |
                             GPIO_PIN_D4 | GPIO_PIN_D5 | GPIO_PIN_D6 | GPIO_PIN_D7,
                             GPIO_VIRTUAL_MODEL_OS_A);
    HAL_GPIO_SetVirtualModel(GPIO0,
                             GPIO_PIN_C3,
                             GPIO_VIRTUAL_MODEL_OS_D);

    /* Test GPIO output */
    HAL_GPIO_SetPinDirection(GPIO0_EXP3, GPIO_PIN_C3, GPIO_OUT);
    level = HAL_GPIO_GetPinLevel(GPIO0_EXP3, GPIO_PIN_C3);
    printf("test gpio 0c3 level = %" PRId32 "\n", level);
    HAL_DelayMs(3000);
    HAL_GPIO_SetPinLevel(GPIO0_EXP3, GPIO_PIN_C3, GPIO_HIGH);
    level = HAL_GPIO_GetPinLevel(GPIO0_EXP3, GPIO_PIN_C3);
    printf("test_gpio 0c3 output high level = %" PRId32 "\n", level);
    HAL_DelayMs(3000);
    HAL_GPIO_SetPinLevel(GPIO0_EXP3, GPIO_PIN_C3, GPIO_LOW);
    level = HAL_GPIO_GetPinLevel(GPIO0_EXP3, GPIO_PIN_C3);
    printf("test_gpio 0c3 output low level = %" PRId32 "\n", level);
    HAL_DelayMs(3000);

    /* Test GPIO interrupt */
    HAL_GPIO_SetPinDirection(GPIO0_EXP3, GPIO_PIN_C3, GPIO_IN);
    HAL_NVIC_SetIRQHandler(GPIO0_EXP3_IRQn, gpio0_exp3_isr);
    HAL_IRQ_HANDLER_SetGpioIRQHandler(GPIO_BANK0_EXP3, GPIO_PIN_C3, c3_call_back, NULL);
    HAL_NVIC_EnableIRQ(GPIO0_EXP3_IRQn);
    HAL_GPIO_SetIntType(GPIO0_EXP3, GPIO_PIN_C3, GPIO_INT_TYPE_EDGE_BOTH);
    HAL_GPIO_EnableIRQ(GPIO0_EXP3, GPIO_PIN_C3);
    printf("test gpio exp interrupt ready\n");
}
#endif

/************************************************/
/*                                              */
/*                  PERF_TEST                   */
/*                                              */
/************************************************/
#ifdef PERF_TEST
#include "benchmark.h"

uint32_t g_sum = 0;

static void perf_test(void)
{
    uint32_t loop = 1000, size = 64 * 1024;
    uint32_t *ptr;
    uint32_t time_start, time_end, time_ms;

    printf("Perftest Start:\n");
    benchmark_main();

    printf("memset test start:\n");
    ptr = (uint32_t *)malloc(size);
    if (ptr) {
        time_start = HAL_GetTick();
        for (int i = 0; i < loop; i++) {
            memset(ptr, i, size);
        }
        time_end = HAL_GetTick();
        time_ms = time_end - time_start;
        printf("memset bw=%" PRId32 "KB/s, time_ms=%d\n",
               1000 * (size * loop / 1024) / time_ms, time_ms);

        /* prevent optimization */
        for (int i = 0; i < size / sizeof(uint32_t); i++) {
            g_sum += ptr[i];
        }
        printf("sum=%d\n", g_sum);
        free(ptr);
    }
    printf("memset test end\n");

    printf("Perftest End:\n");
}
#endif

/************************************************/
/*                                              */
/*                SOFTIRQ_TEST                  */
/*                                              */
/************************************************/
#ifdef SOFTIRQ_TEST
static void soft_isr(void)
{
    printf("softirq_test: enter isr\n");
}

static void softirq_test(void)
{
    printf("softirq_test start\n");
    HAL_NVIC_SetIRQHandler(31, soft_isr);
    HAL_NVIC_EnableIRQ(31);

    HAL_DelayMs(4000);
    HAL_NVIC_SetPendingIRQ(31);
}
#endif

/************************************************/
/*                                              */
/*                SYSTICK_TEST                  */
/*                                              */
/************************************************/
#ifdef SYSTICK_TEST

#define SYSTICK_CORE_RATE 396000000 /* BUS M0 396MHz */

static int systick_int_count = 0;
static uint32_t systick_latency_sum = 0;
static uint32_t systick_latency_max = 0;

static void systick_isr(void)
{
    uint32_t count_reload, count;
    uint32_t latency;

    count_reload = SysTick->VAL;
    count = SYSTICK_CORE_RATE / 100 - 1 - count_reload;
    latency = 1000 * count / (SYSTICK_CORE_RATE / 1000000);
    systick_int_count++;
    systick_latency_sum += latency;
    systick_latency_max = systick_latency_max > latency ? systick_latency_max : latency;
    if (systick_int_count == 100) {
        printf("systick_test: latency=%" PRId32 "ns(count=%" PRId32 ")\n", latency, count);
        printf("systick_test: latency avg=%dns,max=%dns\n", systick_latency_sum / systick_int_count, systick_latency_max);
        systick_int_count = 0;
        systick_latency_sum = 0;
        systick_latency_max = 0;
        HAL_SYSTICK_Disable();
    }
}

static void systick_test(void)
{
    printf("softirq_test start\n");
    HAL_NVIC_SetIRQHandler(SysTick_IRQn, systick_isr);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    HAL_SYSTICK_CLKSourceConfig(HAL_SYSTICK_CLKSRC_CORE);
    HAL_SYSTICK_Config((SYSTICK_CORE_RATE / 100) - 1);
    HAL_SYSTICK_Enable();
}
#endif

/************************************************/
/*                                              */
/*                  TIMER_TEST                  */
/*                                              */
/************************************************/
#ifdef TIMER_TEST
static int timer_int_count = 0;
static uint32_t latency_sum = 0;
static uint32_t latency_max = 0;
struct TIMER_REG *test_timer = TIMER10;

static void timer_isr(void)
{
    uint32_t count;
    uint32_t latency;

    count = (uint32_t)HAL_TIMER_GetCount(test_timer);
    if (count < 1000000) {
        /* 24M timer: 41.67ns per count */
        latency = count * 41;
        timer_int_count++;
        printf("timer_test: latency=%" PRId32 "ns(count=%" PRId32 ")\n", latency, count);
        latency_sum += latency;
        latency_max = latency_max > latency ? latency_max : latency;
    }

    if (timer_int_count == 100) {
        printf("timer_test: latency avg=%dns,max=%dns\n", latency_sum / timer_int_count, latency_max);
        timer_int_count = 0;
        latency_sum = 0;
        latency_max = 0;
        HAL_TIMER_ClrInt(test_timer);
        HAL_TIMER_Stop_IT(test_timer);
    }

    HAL_TIMER_ClrInt(test_timer);
}

static void timer_test(void)
{
    uint64_t start, end;
    uint32_t count;

    printf("timer_test start\n");
    start = HAL_GetSysTimerCount();
    HAL_DelayUs(1000000);
    end = HAL_GetSysTimerCount();
    count = (uint32_t)(end - start);
    printf("sys_timer 1s count: %" PRId32 "(%lld, %lld)\n", count, start, end);

    HAL_TIMER_Init(test_timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(test_timer, 2000000000);
    HAL_TIMER_Start(test_timer);
    start = HAL_TIMER_GetCount(test_timer);
    HAL_DelayUs(1000000);
    end = HAL_TIMER_GetCount(test_timer);
    count = (uint32_t)(end - start);
    printf("test_timer 1s count: %" PRId32 "(%lld, %lld)\n", count, start, end);
    HAL_TIMER_Stop(test_timer);

    HAL_NVIC_SetIRQHandler(TIMER10_IRQn, timer_isr);
    HAL_NVIC_EnableIRQ(TIMER10_IRQn);
    printf("timer_test: 1s interrupt start\n");
    HAL_TIMER_Init(test_timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(test_timer, 24000000);
    HAL_TIMER_Start_IT(test_timer);
}
#endif

/************************************************/
/*                                              */
/*              TIMER_INTMUX_TEST               */
/*                                              */
/************************************************/
#ifdef TIMER_INTMUX_TEST
static int timer_int_count = 0;
static uint32_t latency_sum = 0;
static uint32_t latency_max = 0;
struct TIMER_REG *test_timer = TIMER8;

static void timer_isr(uint32_t irq, void *args)
{
    uint32_t count;
    uint32_t latency;

    count = (uint32_t)HAL_TIMER_GetCount(test_timer);
    /* 24M timer: 41.67ns per count */
    latency = count * 41;
    printf("timer_test: latency=%" PRId32 "ns(count=%" PRId32 ")\n", latency, count);
    timer_int_count++;
    latency_sum += latency;
    latency_max = latency_max > latency ? latency_max : latency;
    if (timer_int_count == 100) {
        printf("timer_test: latency avg=%dns,max=%dns\n", latency_sum / timer_int_count, latency_max);
        timer_int_count = 0;
        latency_sum = 0;
        latency_max = 0;
        HAL_TIMER_ClrInt(test_timer);
        HAL_TIMER_Stop_IT(test_timer);
    }

    HAL_TIMER_ClrInt(test_timer);
}

static void timer_test(void)
{
    uint64_t start, end;
    uint32_t count;

    printf("timer_test start\n");
    start = HAL_GetSysTimerCount();
    HAL_DelayUs(1000000);
    end = HAL_GetSysTimerCount();
    count = (uint32_t)(end - start);
    printf("sys_timer 1s count: %" PRId32 "(%lld, %lld)\n", count, start, end);

    HAL_TIMER_Init(test_timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(test_timer, 2000000000);
    HAL_TIMER_Start(test_timer);
    start = HAL_TIMER_GetCount(test_timer);
    HAL_DelayUs(1000000);
    end = HAL_TIMER_GetCount(test_timer);
    count = (uint32_t)(end - start);
    printf("test_timer 1s count: %" PRId32 "(%lld, %lld)\n", count, start, end);
    HAL_TIMER_Stop(test_timer);

    HAL_INTMUX_SetIRQHandler(TIMER8_IRQn, timer_isr, NULL);
    HAL_INTMUX_EnableIRQ(TIMER8_IRQn);
    printf("timer_test: 1s interrupt start\n");
    HAL_TIMER_Init(test_timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(test_timer, 24000000);
    HAL_TIMER_Start_IT(test_timer);
}
#endif

void test_demo(void)
{
#ifdef GPIO_VIRTUAL_MODEL_TEST
    gpio_virtual_model_test();
#endif

#ifdef PERF_TEST
    perf_test();
#endif

#ifdef SOFTIRQ_TEST
    softirq_test();
#endif

#ifdef SYSTICK_TEST
    systick_test();
#endif

#ifdef TIMER_TEST
    timer_test();
#endif

#ifdef TIMER_INTMUX_TEST
    timer_test();
#endif
}
