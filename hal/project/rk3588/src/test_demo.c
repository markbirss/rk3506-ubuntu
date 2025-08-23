/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"
#include "hal_base.h"
#include <stdlib.h>

/********************* Private MACRO Definition ******************************/
//#define SOFTIRQ_TEST
//#define GPIO_TEST
//#define GPIO_VIRTUAL_MODEL_TEST
//#define RPMSG_LINUX_TEST

/********************* Private Structure Definition **************************/

static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] = {
/* TODO: Config the irqs here.
 * GIC version: GICv3
 * The priority higher than 0x80 is non-secure interrupt.
 */
#ifdef GPIO_TEST
    GIC_AMP_IRQ_CFG_ROUTE(GPIO3_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

#ifdef GPIO_VIRTUAL_MODEL_TEST
    GIC_AMP_IRQ_CFG_ROUTE(GPIO3_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

#ifdef SOFTIRQ_TEST
    GIC_AMP_IRQ_CFG_ROUTE(RSVD0_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

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
static void gpio3_isr(int vector, void *param)
{
    HAL_GPIO_IRQHandler(GPIO3, GPIO_BANK3);
}

static HAL_Status b2_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOB2 callback!\n");

    return HAL_OK;
}

static HAL_Status c0_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOC0 callback!\n");

    return HAL_OK;
}

static void gpio_test(void)
{
    uint32_t level1, level2;

    /* Test GPIO pull */
    printf("test_gpio pull UP\n");
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B2 |
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC0 |
                         PIN_CONFIG_PUL_UP);
    printf("GPIO3B_P: %p = 0x%" PRIx32 "\n", &VCCIO3_5_IOC->GPIO3B_P, VCCIO3_5_IOC->GPIO3B_P);
    printf("GPIO3C_P: %p = 0x%" PRIx32 "\n", &VCCIO3_5_IOC->GPIO3C_P, VCCIO3_5_IOC->GPIO3C_P);
    HAL_DelayMs(3000);
    printf("test_gpio pull DOWN\n");
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B2 |
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC0 |
                         PIN_CONFIG_PUL_DOWN);
    HAL_DelayMs(3000);
    printf("GPIO3B_P: %p = 0x%" PRIx32 "\n", &VCCIO3_5_IOC->GPIO3B_P, VCCIO3_5_IOC->GPIO3B_P);
    printf("GPIO3C_P: %p = 0x%" PRIx32 "\n", &VCCIO3_5_IOC->GPIO3C_P, VCCIO3_5_IOC->GPIO3C_P);

    /* Test GPIO output */
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_B2, GPIO_OUT);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_C0, GPIO_OUT);
    level1 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_B2);
    level2 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_C0);
    printf("test_gpio 3b2 level = %" PRId32 "\n", level1);
    printf("test_gpio 3c0 level = %" PRId32 "\n", level2);
    HAL_DelayMs(3000);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_B2, GPIO_HIGH);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_C0, GPIO_HIGH);
    level1 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_B2);
    level2 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_C0);
    printf("test_gpio 3b2 output high level = %" PRId32 "\n", level1);
    printf("test_gpio 3c0 output high level = %" PRId32 "\n", level2);
    HAL_DelayMs(3000);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_B2, GPIO_LOW);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_C0, GPIO_LOW);
    level1 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_B2);
    level2 = HAL_GPIO_GetPinLevel(GPIO3, GPIO_PIN_C0);
    printf("test_gpio 3b2 output low level = %" PRId32 "\n", level1);
    printf("test_gpio 3c0 output low level = %" PRId32 "\n", level2);
    HAL_DelayMs(3000);

    /* Test GPIO interrupt */
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B2 |
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC0 |
                         PIN_CONFIG_PUL_UP);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_B2, GPIO_IN);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_C0, GPIO_IN);
    HAL_IRQ_HANDLER_SetIRQHandler(GPIO3_IRQn, gpio3_isr, NULL);
    HAL_IRQ_HANDLER_SetGpioIRQHandler(GPIO_BANK3, GPIO_PIN_B2, b2_call_back, NULL);
    HAL_IRQ_HANDLER_SetGpioIRQHandler(GPIO_BANK3, GPIO_PIN_C0, c0_call_back, NULL);
    HAL_GIC_Enable(GPIO3_IRQn);
    HAL_GPIO_SetIntType(GPIO3, GPIO_PIN_B2, GPIO_INT_TYPE_EDGE_FALLING);
    HAL_GPIO_SetIntType(GPIO3, GPIO_PIN_C0, GPIO_INT_TYPE_EDGE_FALLING);
    HAL_GPIO_EnableIRQ(GPIO3, GPIO_PIN_B2);
    HAL_GPIO_EnableIRQ(GPIO3, GPIO_PIN_C0);
    printf("test_gpio interrupt ready\n");
}
#endif

/************************************************/
/*                                              */
/*          GPIO_VIRTUAL_MODEL_TEST             */
/*                                              */
/************************************************/
#ifdef GPIO_VIRTUAL_MODEL_TEST
static void gpio3_isr(int vector, void *param)
{
    HAL_GPIO_IRQHandler(GPIO3, GPIO_BANK3);
}

static void gpio3_exp_isr(int vector, void *param)
{
    HAL_GPIO_IRQHandler(GPIO3_EXP, GPIO_BANK3_EXP);
}

static HAL_Status b2_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOB2 OS_A callback!\n");

    return HAL_OK;
}

static HAL_Status b2_exp_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOB2 OS_B callback!\n");

    return HAL_OK;
}

static HAL_Status c0_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOC0 OS_A callback!\n");

    return HAL_OK;
}

static HAL_Status c0_exp_call_back(eGPIO_bankId bank, uint32_t pin, void *args)
{
    printf("GPIOC0 OS_B callback!\n");

    return HAL_OK;
}

static void gpio_virtual_model_test(void)
{
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B2 |
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC0 |
                         PIN_CONFIG_PUL_UP);
    HAL_GPIO_EnableVirtualModel(GPIO3);
    HAL_GPIO_SetVirtualModel(GPIO3,
                             GPIO_PIN_A0 | GPIO_PIN_A1 | GPIO_PIN_A2 | GPIO_PIN_A3 |
                             GPIO_PIN_A4 | GPIO_PIN_A5 | GPIO_PIN_A6 | GPIO_PIN_A7 |
                             GPIO_PIN_B0 | GPIO_PIN_B1 | GPIO_PIN_B2 | GPIO_PIN_B3 |
                             GPIO_PIN_B4 | GPIO_PIN_B5 | GPIO_PIN_B6 | GPIO_PIN_B7,
                             GPIO_VIRTUAL_MODEL_OS_A);
    HAL_GPIO_SetVirtualModel(GPIO3,
                             GPIO_PIN_C0 | GPIO_PIN_C1 | GPIO_PIN_C2 | GPIO_PIN_C3 |
                             GPIO_PIN_C4 | GPIO_PIN_C5 | GPIO_PIN_C6 | GPIO_PIN_C7 |
                             GPIO_PIN_D0 | GPIO_PIN_D1 | GPIO_PIN_D2 | GPIO_PIN_D3 |
                             GPIO_PIN_D4 | GPIO_PIN_D5 | GPIO_PIN_D6 | GPIO_PIN_D7,
                             GPIO_VIRTUAL_MODEL_OS_B);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_B2, GPIO_IN);
    HAL_GPIO_SetPinDirection(GPIO3_EXP, GPIO_PIN_C0, GPIO_IN);
    HAL_IRQ_HANDLER_SetIRQHandler(GPIO3_IRQn, gpio3_isr, NULL);
    HAL_IRQ_HANDLER_SetIRQHandler(GPIO3_EXP_IRQn, gpio3_exp_isr, NULL);
    HAL_IRQ_HANDLER_SetGpioIRQHandler(GPIO_BANK3, GPIO_PIN_B2, b2_call_back, NULL);
    HAL_IRQ_HANDLER_SetGpioIRQHandler(GPIO_BANK3_EXP, GPIO_PIN_C0, c0_exp_call_back, NULL);
    HAL_GIC_Enable(GPIO3_IRQn);
    HAL_GIC_Enable(GPIO3_EXP_IRQn);
    HAL_GPIO_SetIntType(GPIO3, GPIO_PIN_B2, GPIO_INT_TYPE_EDGE_FALLING);
    HAL_GPIO_SetIntType(GPIO3_EXP, GPIO_PIN_C0, GPIO_INT_TYPE_EDGE_FALLING);
    HAL_GPIO_EnableIRQ(GPIO3, GPIO_PIN_B2);
    HAL_GPIO_EnableIRQ(GPIO3_EXP, GPIO_PIN_C0);
    printf("test_gpio virtual mode interrupt ready\n");
}

#endif

/************************************************/
/*                                              */
/*                SOFTIRQ_TEST                  */
/*                                              */
/************************************************/
#ifdef SOFTIRQ_TEST
static void soft_isr(int vector, void *param)
{
    printf("soft_isr, vector = %d\n", vector);
    HAL_GIC_EndOfInterrupt(vector);
}

static void softirq_test(void)
{
    printf("softirq_test:\n");
    HAL_IRQ_HANDLER_SetIRQHandler(RSVD0_IRQn, soft_isr, NULL);
    HAL_GIC_Enable(RSVD0_IRQn);

    HAL_GIC_SetPending(RSVD0_IRQn);
    HAL_DelayMs(2000);
    HAL_GIC_SetPending(RSVD0_IRQn);
}
#endif

/************************************************/
/*                                              */
/*              RPMSG_LINUX_TEST                */
/*                                              */
/************************************************/
#ifdef RPMSG_LINUX_TEST
#include "rpmsg_lite.h"
#include "rpmsg_ns.h"

// test is CPU0 as master and CPU3 as remote.
#define MASTER_ID   ((uint32_t)0)
#define REMOTE_ID_3 ((uint32_t)3)

// define remote endpoint id for test
#define RPMSG_HAL_REMOTE_TEST3_EPT      0x3003U
#define RPMSG_HAL_REMOTE_TEST_EPT3_NAME "rpmsg-ap3-ch0"

#define RPMSG_HAL_TEST_MSG "Rockchip rpmsg linux test!"

/* TODO: These are defined in the linked script gcc_arm.ld.S */
extern uint32_t __linux_share_rpmsg_start__[];
extern uint32_t __linux_share_rpmsg_end__[];

#define RPMSG_LINUX_MEM_BASE ((uint32_t)&__linux_share_rpmsg_start__)
#define RPMSG_LINUX_MEM_END  ((uint32_t)&__linux_share_rpmsg_end__)
#define RPMSG_LINUX_MEM_SIZE (2UL * RL_VRING_OVERHEAD)

struct rpmsg_block_t {
    uint32_t len;
    uint8_t buffer[496 - 4];
};

struct rpmsg_info_t {
    struct rpmsg_lite_instance *instance;
    struct rpmsg_lite_endpoint *ept;
    uint32_t cb_sta;    // callback status flags
    void * private;
    uint32_t m_ept_id;
};

static void rpmsg_share_mem_check(void)
{
    if ((RPMSG_LINUX_MEM_BASE + RPMSG_LINUX_MEM_SIZE) > RPMSG_LINUX_MEM_END) {
        printf("shared memory size error!\n");
        while (1) {
            ;
        }
    }
}

rpmsg_ns_new_ept_cb rpmsg_ns_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
    uint32_t cpu_id;
    char ept_name[RL_NS_NAME_SIZE];

    cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    strncpy(ept_name, new_ept_name, RL_NS_NAME_SIZE);
    printf("rpmsg remote: new_ept-0x%" PRIx32 " name-%s\n", new_ept, ept_name);
}

static int32_t remote_ept_cb(void *payload, uint32_t payload_len, uint32_t src, void *priv)
{
    int ret;
    uint32_t cpu_id;
    struct rpmsg_info_t *info = (struct rpmsg_info_t *)priv;
    struct rpmsg_block_t *block = (struct rpmsg_block_t *)info->private;

    cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();

    info->m_ept_id = src;
    block->len = payload_len;
    memcpy(block->buffer, payload, payload_len);
    info->cb_sta = 1;
    //printf("rpmsg remote: msg: %s\n", block->buffer);
    ret = rpmsg_lite_send(info->instance, info->ept, info->m_ept_id, RPMSG_HAL_TEST_MSG, strlen(RPMSG_HAL_TEST_MSG), RL_BLOCK);

    if (ret) {
        printf("rpmsg_lite_send error!\n");
    }

    return ret;
}

static void rpmsg_linux_test(void)
{
    uint32_t master_id, remote_id;
    struct rpmsg_info_t *info;
    struct rpmsg_block_t *block;
    uint32_t ept_flags;
    void *ns_cb_data;

    rpmsg_share_mem_check();
    master_id = MASTER_ID;
    remote_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    printf("rpmsg remote: remote core cpu_id-%" PRId32 "\n", remote_id);
//    printf("rpmsg remote: shmem_base-0x%" PRIx32 " shmem_end-%" PRIx32 "\n", RPMSG_LINUX_MEM_BASE, RPMSG_LINUX_MEM_END);

    info = malloc(sizeof(struct rpmsg_info_t));
    if (info == NULL) {
        printf("info malloc error!\n");
        while (1) {
            ;
        }
    }
    info->private = malloc(sizeof(struct rpmsg_block_t));
    if (info->private == NULL) {
        printf("info malloc error!\n");
        while (1) {
            ;
        }
    }

    info->instance = rpmsg_lite_remote_init((void *)RPMSG_LINUX_MEM_BASE, RL_PLATFORM_SET_LINK_ID(master_id, remote_id), RL_NO_FLAGS);
    rpmsg_lite_wait_for_link_up(info->instance);
    printf("rpmsg remote: link up! link_id-0x%" PRIx32 "\n", info->instance->link_id);
    rpmsg_ns_bind(info->instance, rpmsg_ns_cb, &ns_cb_data);
    info->ept = rpmsg_lite_create_ept(info->instance, RPMSG_HAL_REMOTE_TEST3_EPT, remote_ept_cb, info);
    ept_flags = RL_NS_CREATE;
    rpmsg_ns_announce(info->instance, info->ept, RPMSG_HAL_REMOTE_TEST_EPT3_NAME, ept_flags);
    while (info->cb_sta != 1) {
    }
    rpmsg_lite_send(info->instance, info->ept, info->m_ept_id, RPMSG_HAL_TEST_MSG, sizeof(RPMSG_HAL_TEST_MSG), RL_BLOCK);
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

#if defined(SOFTIRQ_TEST) && defined(CPU0)
    softirq_test();
#endif

#if defined(RPMSG_LINUX_TEST) && defined(CPU3)
    rpmsg_linux_test();
#endif
}
