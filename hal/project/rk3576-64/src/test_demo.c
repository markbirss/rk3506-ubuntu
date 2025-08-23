/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"
#include "hal_base.h"
#include <stdlib.h>

/********************* Private MACRO Definition ******************************/
//#define SOFTIRQ_TEST
//#define GPIO_TEST
//#define GPIO_VIRTUAL_MODEL_TEST
//#define RPMSG_LINUX_TEST
//#define IRQ_PREEMPT_TEST

#if defined(IRQ_PREEMPT_TEST) && !defined(HAL_GIC_PREEMPT_FEATURE_ENABLED)

#endif

/********************* Private Structure Definition **************************/

static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] = {
/* TODO: Config the irqs here.
 * GIC version: GICv2
 * The priority higher than 0x80 is non-secure interrupt.
 */
    GIC_AMP_IRQ_CFG_ROUTE(0, 0, 0),   /* sentinel */
};

static struct GIC_IRQ_AMP_CTRL irqConfig = {
    .cpuAff = CPU_GET_AFFINITY(0, 0),
    .defPrio = 0xd0,
    .defRouteAff = CPU_GET_AFFINITY(0, 0),
    .irqsCfg = &irqsConfig[0],
};

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
/*                  GPIO_TEST                   */
/*                                              */
/************************************************/
#ifdef GPIO_TEST
static void gpio_test(void)
{
}
#endif

/************************************************/
/*                                              */
/*          GPIO_VIRTUAL_MODEL_TEST             */
/*                                              */
/************************************************/
#ifdef GPIO_VIRTUAL_MODEL_TEST
static void gpio_virtual_model_test(void)
{
}
#endif

/************************************************/
/*                                              */
/*                SOFTIRQ_TEST                  */
/*                                              */
/************************************************/
#ifdef SOFTIRQ_TEST
#ifdef HAL_GIC_WAIT_LINUX_INIT_ENABLED

#else

#endif
static void softirq_test(void)
{
}
#endif

/************************************************/
/*                                              */
/*              IRQ_PREEMPT_TEST                */
/*                                              */
/************************************************/
#ifdef IRQ_PREEMPT_TEST
static void irq_preempt_test(void)
{
}
#endif

/************************************************/
/*                                              */
/*              RPMSG_LINUX_TEST                */
/*                                              */
/************************************************/
#ifdef RPMSG_LINUX_TEST
static void rpmsg_linux_test(void)
{
}
#endif

/********************* Public Function Definition ****************************/

void TEST_DEMO_GIC_Init(void)
{
    HAL_GIC_Init(&irqConfig);
}

void test_demo(void)
{
#if defined(GPIO_TEST) && defined(CPU0)
    gpio_test();
#endif

#if defined(GPIO_VIRTUAL_MODEL_TEST) && defined(CPU0)
    gpio_virtual_model_test();
#endif

#if defined(SOFTIRQ_TEST) && defined(CPU3)
    softirq_test();
#endif

#if defined(RPMSG_LINUX_TEST) && defined(CPU3)
    rpmsg_linux_test();
#endif

#if defined(IRQ_PREEMPT_TEST) && defined(CPU3)
    irq_preempt_test();
#endif
}