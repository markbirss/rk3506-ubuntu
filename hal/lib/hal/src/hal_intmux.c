/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_INTMUX_MODULE_ENABLED

#ifdef INTMUX_IRQ_INTEN_L_OFFSET
#ifdef __xtensa__
#include <xtensa/xtruntime.h>
#endif

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup INTMUX
 *  @{
 */

/** @defgroup INTMUX_How_To_Use How To Use
 *  @{

 The INTMUX implement can be used as follows:
 - Invoke HAL_INTMUX_EnableIRQ() to enable irq.
 - Invoke HAL_INTMUX_DisableIRQ() to disable irq.
 - Invoke HAL_INTMUX_SetIRQHandler() to set handler for intmux irq.
 - Invoke HAL_INTMUX_Init() to init intmux.
 - Invoke HAL_INTMUX_SetPendingIRQ() to set pending interrupt.
 - Invoke HAL_INTMUX_ClearPendingIRQ() to clear pending interrupt.

 @} */

/** @defgroup INTMUX_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define INTMUX_IRQ_NUM_PER_CH 64
#define INTMUX_IRQ_NUM_PER_ST 32
#define INTMUX_CH_NUM         4
#define INTMUX_IRQ_NUM        (INTMUX_CH_NUM * INTMUX_IRQ_NUM_PER_CH)
#define TO_INTMUX_CH(n)       ((n) / INTMUX_IRQ_NUM_PER_CH)
#define TO_INTMUX_CH_NUM(n)   ((n) % INTMUX_IRQ_NUM_PER_CH)

#define DEFINE_INTMUX_HANDLER_CTRL(channel) \
static struct HAL_INTMUX_HANDLER_CTRL s_intmuxHandler[channel][INTMUX_IRQ_NUM_PER_CH];

#define DEFINE_INTMUX_DISPATCH(ID)                                                       \
static void INTMUX_DispatchCh##ID(void)                                                  \
{                                                                                        \
    uint32_t irqNum;                                                                     \
    uint32_t statusL = INTMUX##ID->IRQ_STATUS_L;                                         \
    uint32_t statusH = INTMUX##ID->IRQ_STATUS_H;                                         \
                                                                                         \
    while (statusL != 0) {                                                               \
        irqNum = __builtin_ffs(statusL) - 1;                                             \
        HAL_ASSERT(s_intmuxHandler[ID][irqNum].handler != NULL);                         \
        s_intmuxHandler[ID][irqNum].handler(irqNum +                                     \
            INTMUX_IRQ_NUM_PER_CH * ID,                                                  \
            s_intmuxHandler[ID][irqNum].args);                                           \
        statusL &= ~(1 << irqNum);                                                       \
    }                                                                                    \
    while (statusH != 0) {                                                               \
        irqNum = __builtin_ffs(statusH) - 1;                                             \
        HAL_ASSERT(s_intmuxHandler[ID][irqNum + INTMUX_IRQ_NUM_PER_ST].handler != NULL); \
        s_intmuxHandler[ID][irqNum + INTMUX_IRQ_NUM_PER_ST].handler(                     \
            irqNum + INTMUX_IRQ_NUM_PER_ST + INTMUX_IRQ_NUM_PER_CH * ID,                 \
            s_intmuxHandler[ID][irqNum + INTMUX_IRQ_NUM_PER_ST].args);                   \
        statusH &= ~(1 << irqNum);                                                       \
    }                                                                                    \
}                                                                                        \
                                                                                         \
static void INTMUX_Extint##ID##Handler(void *arg)                                        \
{                                                                                        \
    INTMUX_DispatchCh##ID();                                                             \
}

/********************* Private Structure Definition **************************/
struct HAL_INTMUX_HANDLER_CTRL {
    HAL_INTMUX_HANDLER handler;
    void *args;
};

/********************* Private Variable Definition ***************************/
DEFINE_INTMUX_HANDLER_CTRL(INTMUX_CH_NUM);

/********************* Private Function Definition ***************************/
static void INTMUX_SetIrqEnable(uint32_t irq, bool enable)
{
    uint32_t irqCh = TO_INTMUX_CH(irq);
    uint32_t irqChNum = TO_INTMUX_CH_NUM(irq);
    volatile uint32_t *reg = NULL;

    switch (irqCh) {
    case 0:
        reg = &INTMUX0->IRQ_INTEN_L;
        break;
    case 1:
        reg = &INTMUX1->IRQ_INTEN_L;
        break;
    case 2:
        reg = &INTMUX2->IRQ_INTEN_L;
        break;
    case 3:
        reg = &INTMUX3->IRQ_INTEN_L;
        break;
    default:

        return;
    }

    if (irqChNum < INTMUX_IRQ_NUM_PER_ST) {
        if (enable) {
            *reg |= 0x1U << irqChNum;
        } else {
            *reg &= ~(0x1U << irqChNum);
        }
    } else {
        reg++;
        if (enable) {
            *reg |= 0x1U << (irqChNum - INTMUX_IRQ_NUM_PER_ST);
        } else {
            *reg &= ~(0x1U << (irqChNum - INTMUX_IRQ_NUM_PER_ST));
        }
    }
}

static void INTMUX_SetIrqForceEnable(uint32_t irq, bool enable)
{
    uint32_t irqCh = TO_INTMUX_CH(irq);
    uint32_t irqChNum = TO_INTMUX_CH_NUM(irq);
    volatile uint32_t *reg = NULL;

    switch (irqCh) {
    case 0:
        reg = &INTMUX0->IRQ_INTFORCE_L;
        break;
    case 1:
        reg = &INTMUX1->IRQ_INTFORCE_L;
        break;
    case 2:
        reg = &INTMUX2->IRQ_INTFORCE_L;
        break;
    case 3:
        reg = &INTMUX3->IRQ_INTFORCE_L;
        break;
    default:

        return;
    }

    if (irqChNum < INTMUX_IRQ_NUM_PER_ST) {
        if (enable) {
            *reg |= 0x1U << irqChNum;
        } else {
            *reg &= ~(0x1U << irqChNum);
        }
    } else {
        reg++;
        if (enable) {
            *reg |= 0x1U << (irqChNum - INTMUX_IRQ_NUM_PER_ST);
        } else {
            *reg &= ~(0x1U << (irqChNum - INTMUX_IRQ_NUM_PER_ST));
        }
    }
}

/* Define interrupt dispatch functions for each channel */
DEFINE_INTMUX_DISPATCH(0)
DEFINE_INTMUX_DISPATCH(1)
DEFINE_INTMUX_DISPATCH(2)
DEFINE_INTMUX_DISPATCH(3)

/** @} */

/********************* Public Function Definition ****************************/

/** @defgroup INTMUX_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Set pending interrupt.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_SetPendingIRQ(uint32_t irq)
{
    HAL_ASSERT(irq < INTMUX_IRQ_NUM);

    INTMUX_SetIrqForceEnable(irq, true);

    return HAL_OK;
}

/**
 * @brief  Clear pending interrupt.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_ClearPendingIRQ(uint32_t irq)
{
    HAL_ASSERT(irq < INTMUX_IRQ_NUM);

    INTMUX_SetIrqForceEnable(irq, false);

    return HAL_OK;
}

/**
 * @brief  Enable intmux irq.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_EnableIRQ(uint32_t irq)
{
    HAL_ASSERT(irq < INTMUX_IRQ_NUM);

    INTMUX_SetIrqEnable(irq, true);

    return HAL_OK;
}

/**
 * @brief  Disable intmux irq.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_DisableIRQ(uint32_t irq)
{
    HAL_ASSERT(irq < INTMUX_IRQ_NUM);

    INTMUX_SetIrqEnable(irq, false);

    return HAL_OK;
}

/**
 * @brief  Intmux direct dispatch
 * @param  irq: irq id
 * @return NONE
 */
void HAL_INTMUX_DirectDispatch(uint32_t irq)
{
}

/**
 * @brief  Set handler for intmux irq
 * @param  irq: irq id.
 * @param  handler: handler callback
 * @param  args: private parameters
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_SetIRQHandler(uint32_t irq, HAL_INTMUX_HANDLER handler, void *args)
{
    uint32_t irqCh = TO_INTMUX_CH(irq);
    uint32_t irqChNum = TO_INTMUX_CH_NUM(irq);

    HAL_ASSERT(irq < INTMUX_IRQ_NUM);
    HAL_ASSERT(handler);

    s_intmuxHandler[irqCh][irqChNum].handler = handler;
    s_intmuxHandler[irqCh][irqChNum].args = args;

    return HAL_OK;
}

/**
 * @brief  Init intmux
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_Init(void)
{
#ifdef __xtensa__
    xtos_set_interrupt_handler(XCHAL_EXTINT0_NUM, INTMUX_Extint0Handler, NULL, NULL);
    xtos_interrupt_enable(XCHAL_EXTINT0_NUM);
    xtos_set_interrupt_handler(XCHAL_EXTINT1_NUM, INTMUX_Extint1Handler, NULL, NULL);
    xtos_interrupt_enable(XCHAL_EXTINT1_NUM);
    xtos_set_interrupt_handler(XCHAL_EXTINT2_NUM, INTMUX_Extint2Handler, NULL, NULL);
    xtos_interrupt_enable(XCHAL_EXTINT2_NUM);
    xtos_set_interrupt_handler(XCHAL_EXTINT3_NUM, INTMUX_Extint3Handler, NULL, NULL);
    xtos_interrupt_enable(XCHAL_EXTINT3_NUM);
#endif

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */
#else
/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup INTMUX
 *  @{
 */

/** @defgroup INTMUX_How_To_Use How To Use
 *  @{

 The INTMUX implement can be used as follows:
 - Invoke HAL_INTMUX_EnableIRQ() to enable irq.
 - Invoke HAL_INTMUX_DisableIRQ() to disable irq.
 - Invoke HAL_INTMUX_SetIRQHandler() to set handler for intmux irq.
 - Invoke HAL_INTMUX_Init() to init intmux.
 - Invoke HAL_INTMUX_DirectDispatch() to direct dispatch irq.

 Use HAL_INTMUX_CUSTOM_DISTRIBUTION_FEATURE_ENABLED to add custom interrupt
 distribution policy. Please define variable g_intmuxFastIrqTable.

 @} */

/** @defgroup INTMUX_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define _TO_INTMUX_NUM(n)   (n - INTMUX_IRQ_START_NUM - NUM_INTERRUPTS)
#define _TO_INTMUX_GROUP(n) (_TO_INTMUX_NUM(n) / INTMUX_NUM_INT_PER_GROUP)
#define _TO_INTMUX_BIT(n)   (_TO_INTMUX_NUM(n) % INTMUX_NUM_INT_PER_GROUP)
#define _TO_INTMUX_OUT(n)   ((_TO_INTMUX_NUM(n) / INTMUX_NUM_INT_PER_OUT) + \
                            INTMUX_OUT_IRQ_START_NUM)

/********************* Private Structure Definition **************************/
struct HAL_INTMUX_HANDLER_CTRL {
    HAL_INTMUX_HANDLER handler;
    void *args;
};

/********************* Private Variable Definition ***************************/
static struct HAL_INTMUX_HANDLER_CTRL s_intmuxHandler[NUM_EXT_INTERRUPTS];

/********************* Private Function Definition ***************************/

/** @} */

/********************* Public Function Definition ****************************/

/** @defgroup INTMUX_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Set pending interrupt.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_SetPendingIRQ(uint32_t irq)
{
    return HAL_OK;
}

/**
 * @brief  Clear pending interrupt.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_ClearPendingIRQ(uint32_t irq)
{
    return HAL_OK;
}

/**
 * @brief  Enable intmux irq.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_EnableIRQ(uint32_t irq)
{
    uint32_t irqGroup, irqBit;

    HAL_ASSERT(irq < TOTAL_INTERRUPTS);
    irqGroup = _TO_INTMUX_GROUP(irq);
    irqBit = _TO_INTMUX_BIT(irq);
#ifdef INTMUX0
    if (_TO_INTMUX_NUM(irq) < INTMUX_NUM_INT_PER_CON) {
        INTMUX0->INT_ENABLE_GROUP[irqGroup] |= 0x1U << irqBit;
    } else {
        INTMUX1->INT_ENABLE_GROUP[irqGroup - INTMUX_NUM_GROUP_PER_CON] |= 0x1U << irqBit;
    }
#else
    INTMUX->INT_ENABLE_GROUP[irqGroup] |= 0x1U << irqBit;
#endif

    return HAL_OK;
}

/**
 * @brief  Disable intmux irq.
 * @param  irq number
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_DisableIRQ(uint32_t irq)
{
    uint32_t irqGroup, irqBit;

    HAL_ASSERT(irq < TOTAL_INTERRUPTS);
    irqGroup = _TO_INTMUX_GROUP(irq);
    irqBit = _TO_INTMUX_BIT(irq);
#ifdef INTMUX0
    if (_TO_INTMUX_NUM(irq) < INTMUX_NUM_INT_PER_CON) {
        INTMUX0->INT_ENABLE_GROUP[irqGroup] &= ~(0x1U << irqBit);
    } else {
        INTMUX1->INT_ENABLE_GROUP[irqGroup - INTMUX_NUM_GROUP_PER_CON] &= ~(0x1U << irqBit);
    }
#else
    INTMUX->INT_ENABLE_GROUP[irqGroup] &= ~(0x1U << irqBit);
#endif

    return HAL_OK;
}

#ifdef HAL_INTMUX_CUSTOM_DISTRIBUTION_FEATURE_ENABLED
static void INTMUX_Dispatch(uint32_t irqOut)
{
    uint32_t irqn, groupFlag;
    int i, j;
    int fastGroup, fastBit;
    bool fastHit = false;

    for (i = 0; g_intmuxFastIrqTable[i] != 0; i++) {
        if (_TO_INTMUX_OUT(g_intmuxFastIrqTable[i]) == irqOut) {
            fastGroup = _TO_INTMUX_GROUP(g_intmuxFastIrqTable[i]);
            fastBit = _TO_INTMUX_BIT(g_intmuxFastIrqTable[i]);
#ifdef INTMUX0
            if (fastGroup < INTMUX_NUM_GROUP_PER_CON) {
                if (INTMUX0->INT_FLAG_GROUP[fastGroup] & HAL_BIT(fastBit)) {
                    irqn = fastGroup * INTMUX_NUM_INT_PER_GROUP + fastBit;
                    if (s_intmuxHandler[irqn].handler) {
                        s_intmuxHandler[irqn].handler(irqn, s_intmuxHandler[irqn].args);
                    }
                    fastHit = true;
                }
            } else {
                if (INTMUX1->INT_FLAG_GROUP[fastGroup - INTMUX_NUM_GROUP_PER_CON] & HAL_BIT(fastBit)) {
                    irqn = fastGroup * INTMUX_NUM_INT_PER_GROUP + fastBit;
                    if (s_intmuxHandler[irqn].handler) {
                        s_intmuxHandler[irqn].handler(irqn, s_intmuxHandler[irqn].args);
                    }
                    fastHit = true;
                }
            }
#else
            if (INTMUX->INT_FLAG_GROUP[fastGroup] & HAL_BIT(fastBit)) {
                irqn = fastGroup * INTMUX_NUM_INT_PER_GROUP + fastBit;
                if (s_intmuxHandler[irqn].handler) {
                    s_intmuxHandler[irqn].handler(irqn, s_intmuxHandler[irqn].args);
                }
                fastHit = true;
            }
#endif
        }
    }

    if (fastHit == false) {
        for (i = irqOut * INTMUX_NUM_GROUP_PER_OUT; i < (irqOut + 1) * INTMUX_NUM_GROUP_PER_OUT; i++) {
#ifdef INTMUX0
            if (irqOut < INTMUX_NUM_OUT_PER_CON) {
                groupFlag = INTMUX0->INT_FLAG_GROUP[i];
            } else {
                groupFlag = INTMUX1->INT_FLAG_GROUP[i - INTMUX_NUM_GROUP_PER_CON];
            }
#else
            groupFlag = INTMUX->INT_FLAG_GROUP[i];
#endif
            if (groupFlag) {
                for (j = 0; j < INTMUX_NUM_INT_PER_GROUP; j++) {
                    if (groupFlag & HAL_BIT(j)) {
                        irqn = i * INTMUX_NUM_INT_PER_GROUP + j;
                        if (s_intmuxHandler[irqn].handler) {
                            s_intmuxHandler[irqn].handler(irqn, s_intmuxHandler[irqn].args);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}
#else
static void INTMUX_Dispatch(uint32_t irqOut)
{
    uint32_t irqn, groupFlag;
    int i, j;

    for (i = irqOut * INTMUX_NUM_GROUP_PER_OUT; i < (irqOut + 1) * INTMUX_NUM_GROUP_PER_OUT; i++) {
#ifdef INTMUX0
        if (irqOut < INTMUX_NUM_OUT_PER_CON) {
            groupFlag = INTMUX0->INT_FLAG_GROUP[i];
        } else {
            groupFlag = INTMUX1->INT_FLAG_GROUP[i - INTMUX_NUM_GROUP_PER_CON];
        }
#else
        groupFlag = INTMUX->INT_FLAG_GROUP[i];
#endif
        if (groupFlag) {
            for (j = 0; j < INTMUX_NUM_INT_PER_GROUP; j++) {
                if (groupFlag & HAL_BIT(j)) {
                    irqn = i * INTMUX_NUM_INT_PER_GROUP + j;
                    if (s_intmuxHandler[irqn].handler) {
                        s_intmuxHandler[irqn].handler(irqn, s_intmuxHandler[irqn].args);
                    }
                }
            }
        }
    }
}
#endif

#ifdef INTMUX_IRQ_OUT0
static void HAL_INTMUX_OUT0_Handler(void)
{
    INTMUX_Dispatch(0);
}
#endif
#ifdef INTMUX_IRQ_OUT1
static void HAL_INTMUX_OUT1_Handler(void)
{
    INTMUX_Dispatch(1);
}
#endif
#ifdef INTMUX_IRQ_OUT2
static void HAL_INTMUX_OUT2_Handler(void)
{
    INTMUX_Dispatch(2);
}
#endif
#ifdef INTMUX_IRQ_OUT3
static void HAL_INTMUX_OUT3_Handler(void)
{
    INTMUX_Dispatch(3);
}
#endif
#ifdef INTMUX_IRQ_OUT4
static void HAL_INTMUX_OUT4_Handler(void)
{
    INTMUX_Dispatch(4);
}
#endif
#ifdef INTMUX_IRQ_OUT5
static void HAL_INTMUX_OUT5_Handler(void)
{
    INTMUX_Dispatch(5);
}
#endif
#ifdef INTMUX_IRQ_OUT6
static void HAL_INTMUX_OUT6_Handler(void)
{
    INTMUX_Dispatch(6);
}
#endif
#ifdef INTMUX_IRQ_OUT7
static void HAL_INTMUX_OUT7_Handler(void)
{
    INTMUX_Dispatch(7);
}
#endif
#ifdef INTMUX_IRQ_OUT8
static void HAL_INTMUX_OUT8_Handler(void)
{
    INTMUX_Dispatch(8);
}
#endif
#ifdef INTMUX_IRQ_OUT9
static void HAL_INTMUX_OUT9_Handler(void)
{
    INTMUX_Dispatch(9);
}
#endif
#ifdef INTMUX_IRQ_OUT10
static void HAL_INTMUX_OUT10_Handler(void)
{
    INTMUX_Dispatch(10);
}
#endif
#ifdef INTMUX_IRQ_OUT11
static void HAL_INTMUX_OUT11_Handler(void)
{
    INTMUX_Dispatch(11);
}
#endif
#ifdef INTMUX_IRQ_OUT12
static void HAL_INTMUX_OUT12_Handler(void)
{
    INTMUX_Dispatch(12);
}
#endif
#ifdef INTMUX_IRQ_OUT13
static void HAL_INTMUX_OUT13_Handler(void)
{
    INTMUX_Dispatch(13);
}
#endif
#ifdef INTMUX_IRQ_OUT14
static void HAL_INTMUX_OUT14_Handler(void)
{
    INTMUX_Dispatch(14);
}
#endif
#ifdef INTMUX_IRQ_OUT15
static void HAL_INTMUX_OUT15_Handler(void)
{
    INTMUX_Dispatch(15);
}
#endif

/**
 * @brief  Intmux direct dispatch
 * @param  irq: irq id
 * @return NONE
 */
void HAL_INTMUX_DirectDispatch(uint32_t irq)
{
    INTMUX_Dispatch(irq);
}

/**
 * @brief  Set handler for intmux irq
 * @param  irq: irq id.
 * @param  handler: handler callback
 * @param  args: private parameters
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_SetIRQHandler(uint32_t irq, HAL_INTMUX_HANDLER handler, void *args)
{
    uint32_t irqn;

    HAL_ASSERT(irq < TOTAL_INTERRUPTS);
    HAL_ASSERT(handler);

    irqn = _TO_INTMUX_NUM(irq);
    s_intmuxHandler[irqn].handler = handler;
    s_intmuxHandler[irqn].args = args;

    return HAL_OK;
}

/**
 * @brief  Init intmux
 * @return HAL_Status.
 */
HAL_Status HAL_INTMUX_Init(void)
{
#ifdef HAL_NVIC_MODULE_ENABLED
#ifdef INTMUX_IRQ_OUT0
    HAL_NVIC_SetIRQHandler(INTMUX_OUT0_IRQn, HAL_INTMUX_OUT0_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT0_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT1
    HAL_NVIC_SetIRQHandler(INTMUX_OUT1_IRQn, HAL_INTMUX_OUT1_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT1_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT2
    HAL_NVIC_SetIRQHandler(INTMUX_OUT2_IRQn, HAL_INTMUX_OUT2_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT2_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT3
    HAL_NVIC_SetIRQHandler(INTMUX_OUT3_IRQn, HAL_INTMUX_OUT3_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT3_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT4
    HAL_NVIC_SetIRQHandler(INTMUX_OUT4_IRQn, HAL_INTMUX_OUT4_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT4_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT5
    HAL_NVIC_SetIRQHandler(INTMUX_OUT5_IRQn, HAL_INTMUX_OUT5_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT5_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT6
    HAL_NVIC_SetIRQHandler(INTMUX_OUT6_IRQn, HAL_INTMUX_OUT6_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT6_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT7
    HAL_NVIC_SetIRQHandler(INTMUX_OUT7_IRQn, HAL_INTMUX_OUT7_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT7_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT8
    HAL_NVIC_SetIRQHandler(INTMUX_OUT8_IRQn, HAL_INTMUX_OUT8_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT8_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT9
    HAL_NVIC_SetIRQHandler(INTMUX_OUT9_IRQn, HAL_INTMUX_OUT9_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT9_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT10
    HAL_NVIC_SetIRQHandler(INTMUX_OUT10_IRQn, HAL_INTMUX_OUT10_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT10_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT11
    HAL_NVIC_SetIRQHandler(INTMUX_OUT11_IRQn, HAL_INTMUX_OUT11_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT11_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT12
    HAL_NVIC_SetIRQHandler(INTMUX_OUT12_IRQn, HAL_INTMUX_OUT12_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT12_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT13
    HAL_NVIC_SetIRQHandler(INTMUX_OUT13_IRQn, HAL_INTMUX_OUT13_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT13_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT14
    HAL_NVIC_SetIRQHandler(INTMUX_OUT14_IRQn, HAL_INTMUX_OUT14_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT14_IRQn);
#endif
#ifdef INTMUX_IRQ_OUT15
    HAL_NVIC_SetIRQHandler(INTMUX_OUT15_IRQn, HAL_INTMUX_OUT15_Handler);
    HAL_NVIC_EnableIRQ(INTMUX_OUT15_IRQn);
#endif
#endif
#ifdef HAL_RISCVIC_MODULE_ENABLED
    HAL_RISCVIC_Init();
#endif

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */
#endif /* INTMUX_IRQ_INTEN_L_OFFSET */
#endif /* HAL_INTMUX_MODULE_ENABLED */
