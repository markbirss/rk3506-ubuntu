/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */
#include "hal_base.h"

#if defined(SOC_RK3506) && defined(HAL_CRU_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup CRU
 *  @{
 */

/** @defgroup CRU_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

static struct PLL_CONFIG PLL_TABLE[] = {
    /* _mhz, _refDiv, _fbDiv, _postdDv1, _postDiv2, _dsmpd, _frac */
    RK_PLL_RATE(1350000000, 4, 225, 1, 1, 1, 0),
    RK_PLL_RATE(1179648000, 1, 49, 1, 1, 0, 2550137),
    RK_PLL_RATE(1000000000, 3, 125, 1, 1, 1, 0),
    RK_PLL_RATE(993484800, 1, 41, 1, 1, 0, 6630355),
    RK_PLL_RATE(983040000, 1, 40, 1, 1, 0, 16106127),
    RK_PLL_RATE(903168000, 1, 75, 2, 1, 0, 4429185),
    RK_PLL_RATE(800000000, 3, 200, 2, 1, 1, 0),
    { 0 /* sentinel */ },
    { /* sentinel */ },
};

static uint32_t s_gpllFreq;
static uint32_t s_v0pllFreq;
static uint32_t s_v1pllFreq;
static uint32_t s_fracUart0Freq;
static uint32_t s_fracUart1Freq;
static uint32_t s_fracVoice0Freq;
static uint32_t s_fracVoice1Freq;
static uint32_t s_fracCom0Freq;
static uint32_t s_fracCom1Freq;
static uint32_t s_fracCom2Freq;
static uint32_t s_intVoice0Freq;
static uint32_t s_intVoice1Freq;
static uint32_t s_intVoice2Freq;
static uint32_t s_sai0MclkIoIn;
static uint32_t s_sai1MclkIoIn;
static uint32_t s_sai2MclkIoIn;
static uint32_t s_sai3MclkIoIn;

static struct PLL_SETUP GPLL = {
    .conOffset0 = &(CRU_PMU->GPLL_CON[0]),
    .conOffset1 = &(CRU_PMU->GPLL_CON[1]),
    .conOffset2 = &(CRU_PMU->GPLL_CON[2]),
    .conOffset3 = &(CRU_PMU->GPLL_CON[3]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 0,
    .lockShift = 10,
    .modeMask = 0x3 << 0,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP V0PLL = {
    .conOffset0 = &(CRU_PMU->VPLL0_CON[0]),
    .conOffset1 = &(CRU_PMU->VPLL0_CON[1]),
    .conOffset2 = &(CRU_PMU->VPLL0_CON[2]),
    .conOffset3 = &(CRU_PMU->VPLL0_CON[3]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 2,
    .lockShift = 10,
    .modeMask = 0x3 << 2,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP V1PLL = {
    .conOffset0 = &(CRU_PMU->VPLL1_CON[0]),
    .conOffset1 = &(CRU_PMU->VPLL1_CON[1]),
    .conOffset2 = &(CRU_PMU->VPLL1_CON[2]),
    .conOffset3 = &(CRU_PMU->VPLL1_CON[3]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 4,
    .lockShift = 10,
    .modeMask = 0x3 << 4,
    .rateTable = PLL_TABLE,
};
/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/

/**
 * @brief Get frac clk freq.
 * @param  clockName: CLOCK_Name id
 * @return clk rate.
 * How to calculate the Frac clk divider:
 *     numerator is frac register[31:16]
 *     denominator is frac register[15:0]
 *     clk rate = pRate * numerator / denominator
 *     for a better jitter, pRate > 20 * rate
 */
static uint32_t HAL_CRU_ClkFracGetFreq(eCLOCK_Name clockName)
{
    uint32_t freq = 0;
    uint32_t muxSrc = 0, divFrac = 0;
    uint32_t n, m, pRate, n_h, m_h;

    switch (clockName) {
    case CLK_FRAC_UART0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_UART0);
        divFrac = CLK_GET_DIV(CLK_FRAC_UART0);
        break;
    case CLK_FRAC_UART1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_UART1);
        divFrac = CLK_GET_DIV(CLK_FRAC_UART1);
        break;
    case CLK_FRAC_VOICE0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_VOICE0);
        divFrac = CLK_GET_DIV(CLK_FRAC_VOICE0);
        break;
    case CLK_FRAC_VOICE1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_VOICE1);
        divFrac = CLK_GET_DIV(CLK_FRAC_VOICE1);
        break;
    case CLK_FRAC_COMMON0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON0);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON0);
        break;
    case CLK_FRAC_COMMON1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON1);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON1);
        break;
    case CLK_FRAC_COMMON2:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON2);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON2);
        break;
    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_FRAC_UART0:
    case CLK_FRAC_UART1:
    case CLK_FRAC_COMMON0:
    case CLK_FRAC_COMMON1:
    case CLK_FRAC_COMMON2:
        pRate = HAL_CRU_MuxGetFreq4(muxSrc, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        HAL_CRU_ClkGetFracDiv(divFrac, &n, &m);
        freq = pRate / m * n;

        return freq;
    case CLK_FRAC_VOICE0:
    case CLK_FRAC_VOICE1:
        pRate = HAL_CRU_MuxGetFreq4(muxSrc, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        HAL_CRU_ClkGetFracDiv(divFrac, &n, &m);
        n_h = (CRU->CRU_CLKSEL_CON[CLK_DIV_GET_REG_OFFSET(divFrac) + 1] & 0xff00) >> 8;
        m_h = (CRU->CRU_CLKSEL_CON[CLK_DIV_GET_REG_OFFSET(divFrac) + 1] & 0xff);
        n = (n_h << 16) | n;
        m = (m_h << 16) | m;
        freq = pRate / m * n;

        return freq;
    default:

        return HAL_INVAL;
    }
}

static uint32_t HAL_CRU_ClkGetOtherFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = 0, freq;
    uint32_t table[12];
    uint32_t uartTable[8] = { PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_fracUart0Freq, s_fracUart1Freq, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq };
    uint32_t voiceTable0[13] = { PLL_INPUT_OSC_RATE, s_intVoice0Freq, s_intVoice1Freq, s_intVoice2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                 s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn, s_sai3MclkIoIn };
    uint32_t pdmTable[14] = { PLL_INPUT_OSC_RATE, s_intVoice0Freq, s_intVoice1Freq, s_intVoice2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                              s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn, s_sai3MclkIoIn };

    switch (clockName) {
    case CLK_FRAC_UART0:
    case CLK_FRAC_UART1:
    case CLK_FRAC_VOICE0:
    case CLK_FRAC_VOICE1:
    case CLK_FRAC_COMMON0:
    case CLK_FRAC_COMMON1:
    case CLK_FRAC_COMMON2:

        return HAL_CRU_ClkFracGetFreq(clockName);
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
    case CLK_UART3:
    case CLK_UART4:
    case CLK_UART5:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, uartTable, 8);
        break;
    case MCLK_SPDIFTX:
    case MCLK_SAI0:
    case MCLK_SAI1:
    case MCLK_SAI2:
    case MCLK_SAI3:
    case MCLK_SAI4:
    case MCLK_ASRC0:
    case MCLK_ASRC1:
    case MCLK_ASRC2:
    case MCLK_ASRC3:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, voiceTable0, 13);
        break;
    case MCLK_PDM:
    case CLKOUT_PDM:
        pdmTable[13] = HAL_CRU_ClkGetFreq(CLK_GPLL_DIV);
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, pdmTable, 14);
        break;
    case LRCK_ASRC0_SRC:
    case LRCK_ASRC1_SRC:
    case LRCK_ASRC0_DST:
    case LRCK_ASRC1_DST:
        table[0] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC0);
        table[1] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC1);
        table[2] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC2);
        table[3] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC3);
        table[4] = HAL_CRU_ClkGetOtherFreq(MCLK_SPDIFTX);
        table[5] = HAL_CRU_ClkGetFreq(MCLK_SPDIFRX);
        table[6] = HAL_CRU_ClkGetOtherFreq(CLKOUT_PDM);
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, table, 12);
        break;
    default:
        break;
    }
    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

static void CRU_Init(void)
{
    s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);
    s_v0pllFreq = HAL_CRU_GetPllFreq(&V0PLL);
    s_v1pllFreq = HAL_CRU_GetPllFreq(&V1PLL);
    s_intVoice0Freq = HAL_CRU_ClkGetFreq(CLK_INT_VOICE0);
    s_intVoice1Freq = HAL_CRU_ClkGetFreq(CLK_INT_VOICE1);
    s_intVoice2Freq = HAL_CRU_ClkGetFreq(CLK_INT_VOICE2);
    s_fracUart0Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_UART0);
    s_fracUart1Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_UART1);
    s_fracVoice0Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_VOICE0);
    s_fracVoice1Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_VOICE1);
    s_fracCom0Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_COMMON0);
    s_fracCom1Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_COMMON1);
    s_fracCom2Freq = HAL_CRU_ClkGetOtherFreq(CLK_FRAC_COMMON2);

    HAL_CRU_DBG("%s: gpll=%" PRId32 ", v0pll=%" PRId32 ", v1pll=%" PRId32 "\n", __func__,
                s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
}

uint32_t HAL_CRU_ClkGetFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t freq = 0, pRate = 0;

    if (!s_gpllFreq || !s_v0pllFreq || !s_v1pllFreq) {
        CRU_Init();
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 ")\n", __func__, clkMux, clkDiv);
    HAL_CRU_DBG("%s: gpll=%" PRId32 ", v0pll=%" PRId32 ", v1pll=%" PRId32 "\n",
                __func__, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);

    switch (clockName) {
    case PLL_GPLL:
        freq = HAL_CRU_GetPllFreq(&GPLL);
        s_gpllFreq = freq;

        return freq;

    case PLL_V0PLL:
        freq = HAL_CRU_GetPllFreq(&V0PLL);
        s_v0pllFreq = freq;

        return freq;

    case PLL_V1PLL:
        freq = HAL_CRU_GetPllFreq(&V1PLL);
        s_v1pllFreq = freq;

        return freq;

    case ARMCLK:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        break;

    case CLK_GPLL_DIV:
    case CLK_MAC:
    case CLK_MAC_OUT:
        pRate = s_gpllFreq;
        break;

    case CLK_GPLL_DIV_100M:
        pRate = HAL_CRU_ClkGetFreq(CLK_GPLL_DIV);
        break;

    case CLK_V0PLL_DIV:
    case CLK_INT_VOICE0:
    case CLK_INT_VOICE2:
        pRate = s_v0pllFreq;
        break;

    case CLK_V1PLL_DIV:
    case CLK_INT_VOICE1:
        pRate = s_v1pllFreq;
        break;

    case ACLK_BUS_ROOT:
    case HCLK_BUS_ROOT:
    case PCLK_BUS_ROOT:
    case ACLK_HSPERI_ROOT:
    case HCLK_LSPERI_ROOT:
    case CLK_PWM1:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, HAL_CRU_ClkGetFreq(CLK_GPLL_DIV), HAL_CRU_ClkGetFreq(CLK_V0PLL_DIV), HAL_CRU_ClkGetFreq(CLK_V1PLL_DIV));
        break;

    case CLK_TSADC:
    case CLK_TSADC_TSEN:
        pRate = PLL_INPUT_OSC_RATE;
        break;

    case CLK_SARADC:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, PLL_INPUT_OSC_RATE, PLL_INPUT_400K_RATE, PLL_INPUT_32K_RATE);
        break;

    case CLK_I2C0:
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_SPI0:
    case CLK_SPI1:
        pRate = HAL_CRU_MuxGetFreq4(clkMux, PLL_INPUT_OSC_RATE, HAL_CRU_ClkGetFreq(CLK_GPLL_DIV), HAL_CRU_ClkGetFreq(CLK_V0PLL_DIV), HAL_CRU_ClkGetFreq(CLK_V1PLL_DIV));
        break;

    case SCLK_FSPI:
    case CCLK_SRC_SDMMC:
    case CLK_DSMC:
    case CLK_FLEXBUS_TX:
    case CLK_FLEXBUS_RX:
    case DCLK_VOP:
    case CLK_REF_OUT0:
    case CLK_REF_OUT1:
        pRate = HAL_CRU_MuxGetFreq4(clkMux, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        break;

    case CLK_PWM0:
        pRate = HAL_CRU_ClkGetFreq(CLK_GPLL_DIV_100M);
        break;

    case CLK_MAC_PTP:
    case MCLK_SPDIFRX:
    case CLK_ASRC0:
    case CLK_ASRC1:
    case CLK_ROOT_DSMC:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        break;
    case CLK_CAN0:
    case CLK_CAN1:
        pRate = HAL_CRU_MuxGetFreq4(clkMux, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);
        break;

    default:

        return HAL_CRU_ClkGetOtherFreq(clockName);
    }

    if (!clkMux && !clkDiv) {
        return 0;
    }

    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 "): freq: %" PRId32 "=%" PRId32 "/%" PRId32 "\n",
                __func__, clkMux, clkDiv, freq, pRate, HAL_CRU_ClkGetDiv(clkDiv));

    return freq;
}

/**
 * @brief Set frac clk freq.
 * @param  clockName: CLOCK_Name id.
 * @param  rate: clk set rate
 * @return HAL_Status.
 * How to calculate the Frac clk divider:
 *     if pRate > 20 * rate, select frac divider
 *     else select normal divider, but the clk rate may be not accurate
 */
static HAL_Status HAL_CRU_ClkFracSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t muxSrc, divFrac, mux;
    uint32_t n = 0, m = 0, pRate = s_gpllFreq;

    switch (clockName) {
    case CLK_FRAC_UART0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_UART0);
        divFrac = CLK_GET_DIV(CLK_FRAC_UART0);
        break;
    case CLK_FRAC_UART1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_UART1);
        divFrac = CLK_GET_DIV(CLK_FRAC_UART1);
        break;
    case CLK_FRAC_VOICE0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_VOICE0);
        divFrac = CLK_GET_DIV(CLK_FRAC_VOICE0);
        break;
    case CLK_FRAC_VOICE1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_VOICE1);
        divFrac = CLK_GET_DIV(CLK_FRAC_VOICE1);
        break;
    case CLK_FRAC_COMMON0:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON0);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON0);
        break;
    case CLK_FRAC_COMMON1:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON1);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON1);
        break;
    case CLK_FRAC_COMMON2:
        muxSrc = CLK_GET_MUX(CLK_FRAC_COMMON2);
        divFrac = CLK_GET_DIV(CLK_FRAC_COMMON2);
        break;
    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_FRAC_UART0:
    case CLK_FRAC_UART1:
    case CLK_FRAC_COMMON0:
    case CLK_FRAC_COMMON1:
    case CLK_FRAC_COMMON2:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 0;
        } else if (!(s_v1pllFreq % rate)) {
            pRate = s_v1pllFreq;
            mux = 2;
        } else if (!(s_v1pllFreq % rate)) {
            pRate = s_v1pllFreq;
            mux = 3;
        } else {
            pRate = s_gpllFreq;
            mux = 1;
        }
        HAL_CRU_FracdivGetConfig(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        HAL_CRU_ClkSetFracDiv(divFrac, n, m);

        return HAL_OK;
    case CLK_FRAC_VOICE0:
    case CLK_FRAC_VOICE1:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 0;
        } else if (!(s_v0pllFreq % rate)) {
            pRate = s_v0pllFreq;
            mux = 2;
        } else if (!(s_v1pllFreq % rate)) {
            pRate = s_v1pllFreq;
            mux = 3;
        } else {
            pRate = s_gpllFreq;
            mux = 1;
        }
        HAL_CRU_FracdivGetConfigV2(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        CRU->CRU_CLKSEL_CON[CLK_DIV_GET_REG_OFFSET(divFrac) + 1] = (((n & 0xff0000) >> 16) << 8) | ((m & 0xff0000) >> 16);
        HAL_CRU_ClkSetFracDiv(divFrac, n & 0xffff, m & 0xffff);

        return HAL_OK;
    default:

        return HAL_INVAL;
    }
}

static HAL_Status HAL_CRU_ClkSetOtherFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = 0, mux = 0, div = 1;
    uint32_t table[12];
    uint32_t uartTable[8] = { PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_fracUart0Freq, s_fracUart1Freq, s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq };
    uint32_t voiceTable0[13] = { PLL_INPUT_OSC_RATE, s_intVoice0Freq, s_intVoice1Freq, s_intVoice2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                                 s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn, s_sai3MclkIoIn };
    uint32_t pdmTable[14] = { PLL_INPUT_OSC_RATE, s_intVoice0Freq, s_intVoice1Freq, s_intVoice2Freq, s_fracVoice0Freq, s_fracVoice1Freq,
                              s_fracCom0Freq, s_fracCom1Freq, s_fracCom2Freq, s_sai0MclkIoIn, s_sai1MclkIoIn, s_sai2MclkIoIn, s_sai3MclkIoIn };

    switch (clockName) {
    case CLK_FRAC_UART0:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracUart0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_UART1:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracUart1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_VOICE0:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracVoice0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_VOICE1:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracVoice1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_COMMON0:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom0Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_COMMON1:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom1Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_FRAC_COMMON2:
        HAL_CRU_ClkFracSetFreq(clockName, rate);
        s_fracCom2Freq = HAL_CRU_ClkFracGetFreq(clockName);

        return HAL_OK;
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
    case CLK_UART3:
    case CLK_UART4:
    case CLK_UART5:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, uartTable, 8, &pRate, 1);
        break;

    case MCLK_SPDIFTX:
    case MCLK_SAI0:
    case MCLK_SAI1:
    case MCLK_SAI2:
    case MCLK_SAI3:
    case MCLK_SAI4:
    case MCLK_ASRC0:
    case MCLK_ASRC1:
    case MCLK_ASRC2:
    case MCLK_ASRC3:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, voiceTable0, 13, &pRate, 1);
        break;

    case MCLK_PDM:
    case CLKOUT_PDM:
        pdmTable[13] = HAL_CRU_ClkGetFreq(CLK_GPLL_DIV);
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, pdmTable, 14, &pRate, 1);
        break;

    case LRCK_ASRC0_SRC:
    case LRCK_ASRC1_SRC:
    case LRCK_ASRC0_DST:
    case LRCK_ASRC1_DST:
        table[0] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC0);
        table[1] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC1);
        table[2] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC2);
        table[3] = HAL_CRU_ClkGetOtherFreq(MCLK_ASRC3);
        table[4] = HAL_CRU_ClkGetOtherFreq(MCLK_SPDIFTX);
        table[5] = HAL_CRU_ClkGetFreq(MCLK_SPDIFRX);
        table[6] = HAL_CRU_ClkGetOtherFreq(CLKOUT_PDM);
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, table, 12, &pRate, 0);
        break;

    default:
        break;
    }

    if (pRate) {
        div = HAL_DIV_ROUND_UP(pRate, rate);
    }
    if (clkDiv) {
        div = (div > CLK_DIV_GET_MAXDIV(clkDiv) ?
               CLK_DIV_GET_MAXDIV(clkDiv) : (div));
        HAL_CRU_ClkSetDiv(clkDiv, div);
    }
    if (clkMux) {
        HAL_CRU_ClkSetMux(clkMux, mux);
    }

    return HAL_OK;
}

HAL_Status HAL_CRU_ClkSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t mux = 0, div = 1, pRate = 0;

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 "): rate=%" PRId32 "\n", __func__, clkMux, clkDiv, rate);
    HAL_CRU_DBG("%s: gpll=%" PRId32 ", v0pll=%" PRId32 ", v1pll=%" PRId32 "\n",
                __func__, s_gpllFreq, s_v0pllFreq, s_v1pllFreq);

    if (!s_gpllFreq || !s_v0pllFreq || !s_v1pllFreq) {
        CRU_Init();
    }

    switch (clockName) {
    case CLK_GPLL_DIV:
    case CLK_GPLL_DIV_100M:
    case CLK_MAC:
    case CLK_MAC_OUT:
        pRate = s_gpllFreq;
        break;
    case CLK_V0PLL_DIV:
        pRate = s_v0pllFreq;
        break;
    case CLK_V1PLL_DIV:
        pRate = s_v1pllFreq;
        break;
    case CLK_INT_VOICE0:
        pRate = s_v0pllFreq;
        div = HAL_DIV_ROUND_UP(pRate, rate);
        HAL_CRU_ClkSetDiv(clkDiv, div);
        s_intVoice0Freq = HAL_CRU_ClkGetFreq(clockName);

        return HAL_OK;
    case CLK_INT_VOICE2:
        pRate = s_v0pllFreq;
        div = HAL_DIV_ROUND_UP(pRate, rate);
        HAL_CRU_ClkSetDiv(clkDiv, div);
        s_intVoice2Freq = HAL_CRU_ClkGetFreq(clockName);

        return HAL_OK;
    case CLK_INT_VOICE1:
        pRate = s_v1pllFreq;
        div = HAL_DIV_ROUND_UP(pRate, rate);
        HAL_CRU_ClkSetDiv(clkDiv, div);
        s_intVoice1Freq = HAL_CRU_ClkGetFreq(clockName);

        return HAL_OK;
    case ACLK_BUS_ROOT:
    case HCLK_BUS_ROOT:
    case PCLK_BUS_ROOT:
    case ACLK_HSPERI_ROOT:
    case HCLK_LSPERI_ROOT:
    case CLK_PWM1:
        mux = HAL_CRU_RoundFreqGetMux3(rate, HAL_CRU_ClkGetFreq(CLK_GPLL_DIV), HAL_CRU_ClkGetFreq(CLK_V0PLL_DIV), HAL_CRU_ClkGetFreq(CLK_V1PLL_DIV), &pRate);
        break;

    case CLK_TSADC:
    case CLK_TSADC_TSEN:
        pRate = PLL_INPUT_OSC_RATE;
        break;

    case CLK_SARADC:
        mux = HAL_CRU_RoundFreqGetMux3(rate, PLL_INPUT_OSC_RATE, PLL_INPUT_400K_RATE, PLL_INPUT_32K_RATE, &pRate);
        break;

    case CLK_I2C0:
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_SPI0:
    case CLK_SPI1:
        mux = HAL_CRU_RoundFreqGetMux4(rate, PLL_INPUT_OSC_RATE, HAL_CRU_ClkGetFreq(CLK_GPLL_DIV), HAL_CRU_ClkGetFreq(CLK_V0PLL_DIV), HAL_CRU_ClkGetFreq(CLK_V1PLL_DIV), &pRate);
        break;

    case SCLK_FSPI:
    case CCLK_SRC_SDMMC:
    case CLK_DSMC:
    case CLK_FLEXBUS_TX:
    case CLK_FLEXBUS_RX:
    case DCLK_VOP:
    case CLK_REF_OUT0:
    case CLK_REF_OUT1:
        mux = HAL_CRU_RoundFreqGetMux4(rate, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq, &pRate);
        break;

    case CLK_PWM0:
        pRate = HAL_CRU_ClkGetFreq(CLK_GPLL_DIV_100M);
        break;

    case CLK_MAC_PTP:
    case MCLK_SPDIFRX:
    case CLK_ASRC0:
    case CLK_ASRC1:
    case CLK_ROOT_DSMC:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_v0pllFreq, s_v1pllFreq, &pRate);
        break;

    case CLK_CAN0:
    case CLK_CAN1:
        mux = HAL_CRU_RoundFreqGetMux4(rate, PLL_INPUT_OSC_RATE, s_gpllFreq, s_v0pllFreq, s_v1pllFreq, &pRate);
        break;

    default:

        return HAL_CRU_ClkSetOtherFreq(clockName, rate);
    }

    if (!clkMux && !clkDiv) {
        return HAL_INVAL;
    }

    if (pRate) {
        div = HAL_DIV_ROUND_UP(pRate, rate);
    }
    if (clkDiv) {
        div = (div > CLK_DIV_GET_MAXDIV(clkDiv) ?
               CLK_DIV_GET_MAXDIV(clkDiv) : (div));
        HAL_CRU_ClkSetDiv(clkDiv, div);
    }
    if (clkMux) {
        HAL_CRU_ClkSetMux(clkMux, mux);
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 "): mux=%" PRId32 ", rate=%" PRId32 ", pRate=%" PRId32 ", div=%" PRId32 ", maxdiv=%d\n",
                __func__, clkMux, clkDiv, mux, rate, pRate, div, CLK_DIV_GET_MAXDIV(clkDiv));

    return HAL_OK;
}

/**
 * @brief wdt glbrst enable.
 * @param  wdtType: wdt reset type.
 * @return HAL_OK.
 * @attention these APIs allow direct use in the HAL layer.
 */
HAL_Status HAL_CRU_WdtGlbRstEnable(eCRU_WdtRstType wdtType)
{
    uint32_t mask = 0, val = 0;

    switch (wdtType) {
    case GLB_RST_FST_WDT0:
        mask = CRU_GLB_RST_CON_CRU_WDT0_CON_MASK | CRU_GLB_RST_CON_CRU_WDT0_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_CRU_WDT0_CON_SHIFT) | (1 << CRU_GLB_RST_CON_CRU_WDT0_EN_SHIFT);
        break;
    case GLB_RST_FST_WDT1:
        mask = CRU_GLB_RST_CON_CRU_WDT1_CON_MASK | CRU_GLB_RST_CON_CRU_WDT1_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_CRU_WDT1_CON_SHIFT) | (1 << CRU_GLB_RST_CON_CRU_WDT1_EN_SHIFT);
        break;
    case GLB_RST_SND_WDT0:
        mask = CRU_GLB_RST_CON_CRU_WDT0_CON_MASK | CRU_GLB_RST_CON_CRU_WDT0_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_CRU_WDT0_EN_SHIFT);
        break;
    case GLB_RST_SND_WDT1:
        mask = CRU_GLB_RST_CON_CRU_WDT1_CON_MASK | CRU_GLB_RST_CON_CRU_WDT1_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_CRU_WDT1_EN_SHIFT);
        break;
    default:

        return HAL_INVAL;
    }

    CRU->GLB_RST_CON = VAL_MASK_WE(mask, val);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* SOC_RK3506 && HAL_CRU_MODULE_ENABLED */
