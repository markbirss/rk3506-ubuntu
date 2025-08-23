/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RV1103B) && defined(HAL_CRU_MODULE_ENABLED)

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

#define OSC_32KHZ 32768

/********************* Private Structure Definition **************************/

static struct PLL_CONFIG PLL_TABLE[] = {
    /* _mhz, _refDiv, _fbDiv, _postdDv1, _postDiv2, _dsmpd, _frac */
    RK_PLL_RATE(1200000000, 1, 100, 2, 1, 1, 0),
    RK_PLL_RATE(1188000000, 1, 99, 2, 1, 1, 0),
    { /* sentinel */ },
};

static uint32_t s_gpllFreq;
static uint32_t s_mclkSaiIo = 0;
static uint32_t s_clkRcOsc = 125000000;

static struct PLL_SETUP GPLL = {
    .conOffset0 = &(TOPCRU->GPLL_CON[0]),
    .conOffset1 = &(TOPCRU->GPLL_CON[1]),
    .conOffset2 = &(TOPCRU->GPLL_CON[2]),
    .conOffset3 = &(TOPCRU->GPLL_CON[3]),
    .modeOffset = &(TOPCRU->MODE_CON00),
    .modeShift = 0,
    .lockShift = 10,
    .modeMask = 0x3 << 0,
    .rateTable = PLL_TABLE,
};

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup CRU_Exported_Functions_Group5 Other Functions
 *  @{
 */

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
    uint32_t mux = CLK_GET_MUX(clockName);
    uint32_t divSrc, divFrac;
    uint32_t n, m, pRate;

    switch (clockName) {
    case CLK_UART0:
        divSrc = CLK_GET_DIV(CLK_UART0_SRC);
        divFrac = CLK_GET_DIV(CLK_UART0_FRAC);
        break;
    case CLK_UART1:
        divSrc = CLK_GET_DIV(CLK_UART1_SRC);
        divFrac = CLK_GET_DIV(CLK_UART1_FRAC);
        break;
    case CLK_UART2:
        divSrc = CLK_GET_DIV(CLK_UART2_SRC);
        divFrac = CLK_GET_DIV(CLK_UART2_FRAC);
        break;
    case MCLK_SAI:
        divSrc = CLK_GET_DIV(CLK_SAI_SRC);
        divFrac = CLK_GET_DIV(CLK_SAI_FRAC);
        break;
    default:

        return HAL_INVAL;
    }

    HAL_CRU_ClkGetFracDiv(divFrac, &n, &m);

    pRate = s_gpllFreq / HAL_CRU_ClkGetDiv(divSrc);

    switch (clockName) {
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
        if (HAL_CRU_ClkGetMux(mux) == 0) {
            freq = pRate;
        } else if (HAL_CRU_ClkGetMux(mux) == 1) {
            freq = (pRate / m) * n;
        } else if (HAL_CRU_ClkGetMux(mux) == 2) {
            freq = PLL_INPUT_OSC_RATE;
        }
        break;
    case MCLK_SAI:
        if (HAL_CRU_ClkGetMux(mux) == 0) {
            freq = pRate;
        } else if (HAL_CRU_ClkGetMux(mux) == 1) {
            freq = (pRate / m) * n;
        } else if (HAL_CRU_ClkGetMux(mux) == 2) {
            freq = s_mclkSaiIo;
        } else {
            freq = PLL_INPUT_OSC_RATE / 2;
        }
        break;
    default:

        return HAL_INVAL;
    }

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
    uint32_t mux = CLK_GET_MUX(clockName);
    uint32_t divSrc, divFrac;
    uint32_t n = 0, m = 0;
    uint32_t gateId, fracGateId;

    switch (clockName) {
    case CLK_UART0:
        divSrc = CLK_GET_DIV(CLK_UART0_SRC);
        divFrac = CLK_GET_DIV(CLK_UART0_FRAC);
        gateId = CLK_UART0_SRC_GATE;
        fracGateId = CLK_UART0_FRAC_GATE;
        break;
    case CLK_UART1:
        divSrc = CLK_GET_DIV(CLK_UART1_SRC);
        divFrac = CLK_GET_DIV(CLK_UART1_FRAC);
        gateId = CLK_UART1_SRC_GATE;
        fracGateId = CLK_UART1_FRAC_GATE;
        break;
    case CLK_UART2:
        divSrc = CLK_GET_DIV(CLK_UART2_SRC);
        divFrac = CLK_GET_DIV(CLK_UART2_FRAC);
        gateId = CLK_UART2_SRC_GATE;
        fracGateId = CLK_UART2_FRAC_GATE;
        break;
    case MCLK_SAI:
        divSrc = CLK_GET_DIV(CLK_SAI_SRC);
        divFrac = CLK_GET_DIV(CLK_SAI_FRAC);
        gateId = CLK_SAI_GATE;
        fracGateId = CLK_SAI_FRAC_GATE;
        break;
    default:

        return HAL_INVAL;
    }

    HAL_CRU_ClkEnable(gateId);
    HAL_CRU_ClkEnable(fracGateId);

    switch (clockName) {
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
        if (PLL_INPUT_OSC_RATE == rate) {
            HAL_CRU_ClkSetMux(mux, 2);
            HAL_CRU_ClkDisable(gateId);
        } else if ((!(s_gpllFreq % rate)) && ((s_gpllFreq / rate) < 31)) {
            HAL_CRU_ClkSetDiv(divSrc, s_gpllFreq / rate);
            HAL_CRU_ClkSetMux(mux, 0);
            HAL_CRU_ClkDisable(fracGateId);
        } else {
            HAL_CRU_FracdivGetConfig(rate, s_gpllFreq, &n, &m);
            HAL_CRU_ClkSetDiv(divSrc, 1);
            HAL_CRU_ClkSetFracDiv(divFrac, n, m);
            HAL_CRU_ClkSetMux(mux, 1);
        }
        break;
    case MCLK_SAI:
        if (PLL_INPUT_OSC_RATE / 2 == rate) {
            HAL_CRU_ClkSetMux(mux, 3);
            HAL_CRU_ClkDisable(gateId);
        } else if ((!(s_gpllFreq % rate)) && ((s_gpllFreq / rate) < 31)) {
            HAL_CRU_ClkSetDiv(divSrc, s_gpllFreq / rate);
            HAL_CRU_ClkSetMux(mux, 0);
            HAL_CRU_ClkDisable(fracGateId);
        } else if (rate == s_mclkSaiIo) {
            HAL_CRU_ClkSetMux(mux, 2);
            HAL_CRU_ClkDisable(gateId);
        } else {
            HAL_CRU_FracdivGetConfig(rate, s_gpllFreq, &n, &m);
            HAL_CRU_ClkSetDiv(divSrc, 1);
            HAL_CRU_ClkSetFracDiv(divFrac, n, m);
            HAL_CRU_ClkSetMux(mux, 1);
        }
        break;
    default:

        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief init pll freq.
 */
static void CRU_InitPlls(void)
{
    TOPCRU->CRU_CLKSEL_CON[31] = VAL_MASK_WE(TOPCRU_CLKSEL_CON31_LSCLK_PERI_SRC_SEL_MASK, 1 << TOPCRU_CLKSEL_CON31_LSCLK_PERI_SRC_SEL_SHIFT);
    TOPCRU->CRU_CLKSEL_CON[37] = VAL_MASK_WE(TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_DIV_MASK, 1 << TOPCRU_CLKSEL_CON37_CLK_CORE_GPLL_SRC_DIV_SHIFT);
    TOPCRU->GPLL_CON[0] = VAL_MASK_WE(TOPCRU_CRU_GPLL_CON0_POSTDIV1_MASK, 1 << TOPCRU_CRU_GPLL_CON0_POSTDIV1_SHIFT);
}

/**
 * @brief Get clk freq.
 * @param  clockName: CLOCK_Name id.
 * @return rate.
 * @attention these APIs allow direct use in the HAL layer.
 */
uint32_t HAL_CRU_ClkGetFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = 0, freq;

    if (!s_gpllFreq) {
        CRU_InitPlls();
        s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);
    }

    switch (clockName) {
    case PLL_GPLL:
        freq = HAL_CRU_GetPllFreq(&GPLL);
        s_gpllFreq = freq;

        return freq;
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
    case MCLK_SAI:
        freq = HAL_CRU_ClkFracGetFreq(clockName);

        return freq;
    case CLK_TSADC:
    case CLK_TSADC_TSEN:
    case CLK_SARADC:
        pRate = PLL_INPUT_OSC_RATE;
        break;

    case CCLK_EMMC:
    case CCLK_SDMMC0:
    case CCLK_SDMMC1:
    case SCLK_SFC_2X:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, PLL_INPUT_OSC_RATE);

        break;
    case LSCLK_PERI_SRC:
    case DCLK_VICAP_SRC:
        return HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 4, s_gpllFreq / 6);
    case ACLK_PERI_SRC:
        return HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq / 2, s_gpllFreq * 2 / 5, s_gpllFreq / 3);
    case HCLK_HPMCU_SRC:
    case CLK_CORE_RGA:
        return HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 3, s_gpllFreq / 4);

    case CLK_I2C0:
        return HAL_CRU_MuxGetFreq2(CLK_GET_MUX(CLK_I2C_PMU), s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_I2C3:
    case CLK_I2C4:
        return HAL_CRU_MuxGetFreq2(CLK_GET_MUX(CLK_I2C_PERI), s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
    case CLK_SPI0:
        return HAL_CRU_MuxGetFreq4(clkMux, s_gpllFreq / 6, s_gpllFreq / 12, s_gpllFreq / 24, PLL_INPUT_OSC_RATE);
    case CLK_PWM0:
    case CLK_PWM1:
    case CLK_PWM2:
        return HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
    case DCLK_DECOM:
        return HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq * 2 / 5, s_gpllFreq / 3, s_gpllFreq / 4);
    case CLK_CORE_CRYPTO:
    case CLK_PKA_CRYPTO:
        return HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq / 4, s_gpllFreq / 6, s_gpllFreq / 12);
    case MCLK_SAI_OUT2IO:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, HAL_CRU_ClkGetFreq(MCLK_SAI), PLL_INPUT_OSC_RATE / 2);
        break;
    case CLK_ISP_GPLL_SRC:
        pRate = s_gpllFreq;
        break;
    case CLK_CORE_ISP:
        return HAL_CRU_ClkGetFreq(CLK_ISP_GPLL_SRC);
    case LSCLK_VI_ROOT:
        return HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 6, s_gpllFreq / 12);
    case CLK_REF_MIPI0_SRC:
    case CLK_REF_MIPI1_SRC:
        pRate = s_gpllFreq / 2;
        break;
    case CLK_MIPI0_OUT2IO:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, HAL_CRU_ClkGetFreq(CLK_REF_MIPI0_SRC), PLL_INPUT_OSC_RATE);
        break;
    case CLK_MIPI1_OUT2IO:
        pRate = HAL_CRU_MuxGetFreq2(clkMux, HAL_CRU_ClkGetFreq(CLK_REF_MIPI1_SRC), PLL_INPUT_OSC_RATE);
        break;
    case SCLK_SFC_2X_PMU1:
        return HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq / 12, s_clkRcOsc);
    default:
        break;
    }

    if ((clkMux == 0) && (clkDiv == 0)) {
        return 0;
    }

    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

/**
 * @brief Set clk freq.
 * @param  clockName: CLOCK_Name id.
 * @param  rate: clk rate.
 * @return HAL_Status.
 * @attention these APIs allow direct use in the HAL layer.
 */
HAL_Status HAL_CRU_ClkSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    HAL_Status error = HAL_OK;
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t mux = 0, div = 0, pRate = 0;

    if (!s_gpllFreq) {
        CRU_InitPlls();
        s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);
    }

    switch (clockName) {
    case PLL_GPLL:
        error = HAL_CRU_SetPllFreq(&GPLL, rate);
        s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);

        return error;
    case CLK_UART0:
    case CLK_UART1:
    case CLK_UART2:
    case MCLK_SAI:
        error = HAL_CRU_ClkFracSetFreq(clockName, rate);

        return error;
    case CLK_TSADC:
    case CLK_TSADC_TSEN:
    case CLK_SARADC:
        pRate = PLL_INPUT_OSC_RATE;
        break;

    case CCLK_EMMC:
    case CCLK_SDMMC0:
    case CCLK_SDMMC1:
    case SCLK_SFC_2X:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;
    case LSCLK_PERI_SRC:
    case DCLK_VICAP_SRC:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 4, s_gpllFreq / 6);
        break;
    case ACLK_PERI_SRC:
        mux = HAL_CRU_FreqGetMux3(rate, s_gpllFreq / 2, s_gpllFreq * 2 / 5, s_gpllFreq / 3);
        break;
    case HCLK_HPMCU_SRC:
    case CLK_CORE_RGA:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 3, s_gpllFreq / 4);
        break;
    case CLK_I2C0:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
        clkMux = CLK_GET_MUX(CLK_I2C_PMU);
        break;
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_I2C3:
    case CLK_I2C4:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
        clkMux = CLK_GET_MUX(CLK_I2C_PERI);
        break;
    case CLK_SPI0:
        mux = HAL_CRU_FreqGetMux4(rate, s_gpllFreq / 6, s_gpllFreq / 12, s_gpllFreq / 24, PLL_INPUT_OSC_RATE);
        break;
    case CLK_PWM0:
    case CLK_PWM1:
    case CLK_PWM2:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 12, PLL_INPUT_OSC_RATE);
        break;
    case DCLK_DECOM:
        mux = HAL_CRU_FreqGetMux3(rate, s_gpllFreq * 2 / 5, s_gpllFreq / 3, s_gpllFreq / 4);
        break;

    case CLK_CORE_CRYPTO:
    case CLK_PKA_CRYPTO:
        mux = HAL_CRU_FreqGetMux3(rate, s_gpllFreq / 4, s_gpllFreq / 6, s_gpllFreq / 12);
        break;
    case MCLK_SAI_OUT2IO:
        pRate = HAL_CRU_RoundFreqGetMux2(clkMux, HAL_CRU_ClkGetFreq(MCLK_SAI), PLL_INPUT_OSC_RATE / 2, &pRate);
        break;
    case CLK_ISP_GPLL_SRC:
        pRate = s_gpllFreq;
        break;
    case CLK_CORE_ISP:
        return HAL_CRU_ClkSetFreq(CLK_ISP_GPLL_SRC, rate);
    case LSCLK_VI_ROOT:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 6, s_gpllFreq / 12);
        break;
    case CLK_REF_MIPI0_SRC:
    case CLK_REF_MIPI1_SRC:
        pRate = s_gpllFreq / 2;
        break;
    case CLK_MIPI0_OUT2IO:
        if (rate == PLL_INPUT_OSC_RATE) {
            mux = 1;
        } else {
            mux = 0;
            HAL_CRU_ClkSetFreq(CLK_REF_MIPI0_SRC, rate);
        }
        break;
    case CLK_MIPI1_OUT2IO:
        if (rate == PLL_INPUT_OSC_RATE) {
            mux = 1;
        } else {
            mux = 0;
            HAL_CRU_ClkSetFreq(CLK_REF_MIPI1_SRC, rate);
        }
        break;
    case SCLK_SFC_2X_PMU1:
        mux = HAL_CRU_FreqGetMux2(rate, s_gpllFreq / 12, s_clkRcOsc);
        break;

    default:
        break;
    }

    if ((clkMux == 0) && (clkDiv == 0)) {
        return HAL_INVAL;
    }

    div = HAL_DIV_ROUND_UP(pRate, rate);
    if (clkMux) {
        HAL_CRU_ClkSetMux(clkMux, mux);
    }
    if (clkDiv) {
        HAL_CRU_ClkSetDiv(clkDiv, div);
    }

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
    uint32_t mask = TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_EN_MASK | TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_CTRL_MASK;
    uint32_t val = 1 << TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_EN_SHIFT;

    switch (wdtType) {
    case GLB_RST_FST_WDT0:
        val |= (1 << TOPCRU_CRU_GLB_RST_CON_WDT_GLB_SRST_CTRL_SHIFT);
        break;
    case GLB_RST_SND_WDT0:
        break;
    default:

        return HAL_INVAL;
    }

    TOPCRU->CRU_GLB_RST_CON = VAL_MASK_WE(mask, val);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* SOC_RV1103B && HAL_CRU_MODULE_ENABLED */
