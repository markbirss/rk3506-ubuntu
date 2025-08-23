/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(PERICRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VEPUCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(NPUCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(VICRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(CORECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDRCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SUBDDRCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(TOPCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU0CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU1CRU_BASE, 0x300, 0x800, 0xa00),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI0,
    .sclkGate = SCLK_SFC_2X_SRC_GATE,
    .hclkGate = HCLK_SFC_GATE,
    .xipClkGate = HCLK_SFC_XIP_GATE,
    .sclkID = SCLK_SFC_2X,
    .irqNum = FSPI0_IRQn,
    .xipMemCode = XIP_MAP0_BASE0,
    .xipMemData = XIP_MAP0_BASE0,
    .maxDllCells = 0xFF,
    .xmmcDev[0] =
    {
        .type = DEV_NOR,
    },
};

struct HAL_FSPI_HOST g_fspi1Dev =
{
    .instance = FSPI1,
    .sclkGate = SCLK_SFC_2X_PMU1_GATE,
    .hclkGate = HCLK_SFC_PMU1_GATE,
    .xipClkGate = HCLK_SFC_XIP_PMU1_GATE,
    .sclkID = SCLK_SFC_2X_PMU1,
    .irqNum = FSPI1_IRQn,
    .xipMemCode = XIP_MAP0_BASE1,
    .xipMemData = XIP_MAP0_BASE1,
    .maxDllCells = 0x7F,
    .xmmcDev[0] =
    {
        .type = DEV_PSRAM,
    },
};
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .irqNum = I2C0_IRQn,
    .clkID = CLK_I2C0,
    .clkGateID = CLK_I2C0_GATE,
    .pclkGateID = PCLK_I2C0_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C0,
};

const struct HAL_I2C_DEV g_i2c1Dev =
{
    .pReg = I2C1,
    .irqNum = I2C1_IRQn,
    .clkID = CLK_I2C1,
    .clkGateID = CLK_I2C1_GATE,
    .pclkGateID = PCLK_I2C1_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C1,
};

const struct HAL_I2C_DEV g_i2c2Dev =
{
    .pReg = I2C2,
    .irqNum = I2C2_IRQn,
    .clkID = CLK_I2C2,
    .clkGateID = CLK_I2C2_GATE,
    .pclkGateID = PCLK_I2C2_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C2,
};

const struct HAL_I2C_DEV g_i2c3Dev =
{
    .pReg = I2C3,
    .irqNum = I2C3_IRQn,
    .clkID = CLK_I2C3,
    .clkGateID = CLK_I2C3_GATE,
    .pclkGateID = PCLK_I2C3_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C3,
};

const struct HAL_I2C_DEV g_i2c4Dev =
{
    .pReg = I2C4,
    .irqNum = I2C4_IRQn,
    .clkID = CLK_I2C4,
    .clkGateID = CLK_I2C4_GATE,
    .pclkGateID = PCLK_I2C4_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C4,
};
#endif

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PWM0,
    .clkGateID = CLK_PWM0_SRC_GATE,
    .pclkGateID = PCLK_PWM0_GATE,
    .irqNum[0] = PWM0_CH0_IRQn,
    .irqNum[1] = PWM0_CH1_IRQn,
    .irqNum[2] = PWM0_CH2_IRQn,
    .irqNum[3] = PWM0_CH3_IRQn,
};

const struct HAL_PWM_DEV g_pwm1Dev =
{
    .pReg = PWM1,
    .clkID = CLK_PWM1,
    .clkGateID = CLK_PWM1_SRC_GATE,
    .pclkGateID = PCLK_PWM1_GATE,
    .irqNum[0] = PWM1_CH0_IRQn,
    .irqNum[1] = PWM1_CH1_IRQn,
    .irqNum[2] = PWM1_CH2_IRQn,
    .irqNum[3] = PWM1_CH3_IRQn,
};

const struct HAL_PWM_DEV g_pwm2Dev =
{
    .pReg = PWM2,
    .clkID = CLK_PWM2,
    .clkGateID = CLK_PWM2_SRC_GATE,
    .pclkGateID = PCLK_PWM2_GATE,
    .irqNum[0] = PWM2_CH0_IRQn,
    .irqNum[1] = PWM2_CH1_IRQn,
    .irqNum[2] = PWM2_CH2_IRQn,
    .irqNum[3] = PWM2_CH3_IRQn,
};
#endif

#ifdef HAL_SPI_MODULE_ENABLED
const struct HAL_SPI_DEV g_spi0Dev = {
    .base = SPI0_BASE,
    .clkId = CLK_SPI0,
    .clkGateID = CLK_SPI0_SRC_GATE,
    .pclkGateID = PCLK_SPI0_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI0_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI0_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI0_BASE + 0x400,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI0_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI0_BASE + 0x800,
    },
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = CLK_UART0,
    .sclkGateID = SCLK_UART0_SRC_GATE,
    .pclkGateID = PCLK_UART0_GATE,
#endif
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
#ifdef HAL_CRU_MODULE_ENABLED
    .sclkID = CLK_UART2,
    .sclkGateID = SCLK_UART2_SRC_GATE,
    .pclkGateID = PCLK_UART2_GATE,
#endif
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
};
#endif

void BSP_Init(void)
{
}
