/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */
#include "hal_base.h"

#if defined(SOC_RK3576) && defined(HAL_CRU_MODULE_ENABLED)

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

static struct PLL_CONFIG PLL_TABLE[] =
{
/* _mhz, _p, _m, _s, _k */
    RK3588_PLL_RATE(2016000000, 2, 336, 1, 0),
    RK3588_PLL_RATE(1920000000, 2, 320, 1, 0),
    RK3588_PLL_RATE(1800000000, 2, 300, 1, 0),
    RK3588_PLL_RATE(1608000000, 2, 268, 1, 0),
    RK3588_PLL_RATE(1416000000, 2, 236, 1, 0),
    RK3588_PLL_RATE(1200000000, 2, 200, 1, 0),
    RK3588_PLL_RATE(1188000000, 2, 198, 1, 0),
    RK3588_PLL_RATE(1100000000, 3, 550, 2, 0),
    RK3588_PLL_RATE(1008000000, 2, 336, 2, 0),
    RK3588_PLL_RATE(1000000000, 3, 500, 2, 0),
    RK3588_PLL_RATE(983040000, 4, 655, 2, 23592),
    RK3588_PLL_RATE(955520000, 3, 477, 2, 49806),
    RK3588_PLL_RATE(903168000, 6, 903, 2, 11009),
    RK3588_PLL_RATE(816000000, 2, 272, 2, 0),
    RK3588_PLL_RATE(786432000, 2, 262, 2, 9437),
    RK3588_PLL_RATE(786000000, 1, 131, 2, 0),
    RK3588_PLL_RATE(785560000, 3, 392, 2, 51117),
    RK3588_PLL_RATE(722534400, 8, 963, 2, 24850),
    RK3588_PLL_RATE(594000000, 2, 198, 2, 0),
    RK3588_PLL_RATE(408000000, 2, 272, 3, 0),
    RK3588_PLL_RATE(297000000, 2, 198, 3, 0),
    RK3588_PLL_RATE(216000000, 2, 288, 4, 0),
    RK3588_PLL_RATE(96000000, 2, 256, 5, 0),
    { /* sentinel */ },
};

static struct PLL_CONFIG PPLL_TABLE[] =
{
    /* _mhz, _p, _m, _s, _k */
    RK3588_PLL_RATE(1300000000, 3, 325, 2, 0),
    { /* sentinel */ },
};

static struct PLL_SETUP LPLL =
{
    .conOffset0 = &(CCICRU->LPLL_CON[0]),
    .conOffset1 = &(CCICRU->LPLL_CON[1]),
    .conOffset2 = &(CCICRU->LPLL_CON[2]),
    .conOffset3 = &(CCICRU->LPLL_CON[3]),
    .conOffset6 = &(CCICRU->LPLL_CON[6]),
    .modeOffset = &(LITCORECRU->MODE_CON00),
    .modeShift = 0,
    .lockShift = 15,
    .modeMask = 0x3,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP BPLL =
{
    .conOffset0 = &(CRU->BPLL_CON[0]),
    .conOffset1 = &(CRU->BPLL_CON[1]),
    .conOffset2 = &(CRU->BPLL_CON[2]),
    .conOffset3 = &(CRU->BPLL_CON[3]),
    .conOffset6 = &(CRU->BPLL_CON[6]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 0,
    .lockShift = 15,
    .modeMask = 0x3,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP CPLL =
{
    .conOffset0 = &(CRU->CPLL_CON[0]),
    .conOffset1 = &(CRU->CPLL_CON[1]),
    .conOffset2 = &(CRU->CPLL_CON[2]),
    .conOffset3 = &(CRU->CPLL_CON[3]),
    .conOffset6 = &(CRU->CPLL_CON[6]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 8,
    .lockShift = 15,
    .modeMask = 0x3 << 8,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP GPLL =
{
    .conOffset0 = &(CRU->GPLL_CON[0]),
    .conOffset1 = &(CRU->GPLL_CON[1]),
    .conOffset2 = &(CRU->GPLL_CON[2]),
    .conOffset3 = &(CRU->GPLL_CON[3]),
    .conOffset6 = &(CRU->GPLL_CON[6]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 2,
    .lockShift = 15,
    .modeMask = 0x3 << 2,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP VPLL =
{
    .conOffset0 = &(CRU->VPLL_CON[0]),
    .conOffset1 = &(CRU->VPLL_CON[1]),
    .conOffset2 = &(CRU->VPLL_CON[2]),
    .conOffset3 = &(CRU->VPLL_CON[3]),
    .conOffset6 = &(CRU->VPLL_CON[6]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 4,
    .lockShift = 15,
    .modeMask = 0x3 << 4,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP AUPLL =
{
    .conOffset0 = &(CRU->AUPLL_CON[0]),
    .conOffset1 = &(CRU->AUPLL_CON[1]),
    .conOffset2 = &(CRU->AUPLL_CON[2]),
    .conOffset3 = &(CRU->AUPLL_CON[3]),
    .conOffset6 = &(CRU->AUPLL_CON[6]),
    .modeOffset = &(CRU->MODE_CON00),
    .modeShift = 6,
    .lockShift = 15,
    .modeMask = 0x3 << 6,
    .rateTable = PLL_TABLE,
};

static struct PLL_SETUP PPLL =
{
    .conOffset0 = &(PHPTOPCRU->PPLL_CON[0]),
    .conOffset1 = &(PHPTOPCRU->PPLL_CON[1]),
    .conOffset2 = &(PHPTOPCRU->PPLL_CON[2]),
    .conOffset3 = &(PHPTOPCRU->PPLL_CON[3]),
    .conOffset6 = &(PHPTOPCRU->PPLL_CON[6]),
    .lockShift = 15,
    .rateTable = PPLL_TABLE,
};

/********************* Private Variable Definition ***************************/

static uint32_t s_lpllFreq;
static uint32_t s_bpllFreq;
static uint32_t s_cpllFreq;
static uint32_t s_gpllFreq;
static uint32_t s_vpllFreq;
static uint32_t s_ppllFreq;
static uint32_t s_aupllFreq;
static uint32_t s_fracUart0Freq;
static uint32_t s_fracUart1Freq;
static uint32_t s_fracUart2Freq;
static uint32_t s_fracAudio0Freq;
static uint32_t s_fracAudio1Freq;
static uint32_t s_fracAudio2Freq;
static uint32_t s_fracAudio3Freq;
static uint32_t s_intAudio0Freq;
static uint32_t s_intAudio1Freq;
static uint32_t s_intAudio2Freq;

/********************* Private Function Definition ***************************/

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
    uint32_t n, m, pRate;

    switch (clockName) {
    case CLK_UART_FRAC_0:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_0);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_0);
        break;
    case CLK_UART_FRAC_1:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_1);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_1);
        break;
    case CLK_UART_FRAC_2:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_2);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_2);
        break;
    case CLK_AUDIO_FRAC_0:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_0);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_0);
        break;
    case CLK_AUDIO_FRAC_1:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_1);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_1);
        break;
    case CLK_AUDIO_FRAC_2:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_2);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_2);
        break;
    case CLK_AUDIO_FRAC_3:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_3);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_3);
        break;
    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_UART_FRAC_0:
    case CLK_UART_FRAC_1:
    case CLK_UART_FRAC_2:
    case CLK_AUDIO_FRAC_0:
    case CLK_AUDIO_FRAC_1:
    case CLK_AUDIO_FRAC_2:
    case CLK_AUDIO_FRAC_3:
        pRate = HAL_CRU_MuxGetFreq4(muxSrc, s_gpllFreq, s_cpllFreq, s_aupllFreq, PLL_INPUT_OSC_RATE);
        HAL_CRU_ClkGetFracDiv(divFrac, &n, &m);
        freq = pRate / m * n;

        return freq;
    default:

        return HAL_INVAL;
    }
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
    case CLK_UART_FRAC_0:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_0);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_0);
        break;
    case CLK_UART_FRAC_1:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_1);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_1);
        break;
    case CLK_UART_FRAC_2:
        muxSrc = CLK_GET_MUX(CLK_UART_FRAC_2);
        divFrac = CLK_GET_DIV(CLK_UART_FRAC_2);
        break;
    case CLK_AUDIO_FRAC_0:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_0);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_0);
        break;
    case CLK_AUDIO_FRAC_1:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_1);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_1);
        break;
    case CLK_AUDIO_FRAC_2:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_2);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_2);
        break;
    case CLK_AUDIO_FRAC_3:
        muxSrc = CLK_GET_MUX(CLK_AUDIO_FRAC_3);
        divFrac = CLK_GET_DIV(CLK_AUDIO_FRAC_3);
        break;
    default:

        return HAL_INVAL;
    }

    switch (clockName) {
    case CLK_UART_FRAC_0:
    case CLK_UART_FRAC_1:
    case CLK_UART_FRAC_2:
    case CLK_AUDIO_FRAC_0:
    case CLK_AUDIO_FRAC_1:
    case CLK_AUDIO_FRAC_2:
    case CLK_AUDIO_FRAC_3:
        if ((!(PLL_INPUT_OSC_RATE % rate))) {
            pRate = PLL_INPUT_OSC_RATE;
            mux = 3;
        } else if (!(s_cpllFreq % rate)) {
            pRate = s_cpllFreq;
            mux = 1;
        } else if (!(s_aupllFreq % rate)) {
            pRate = s_aupllFreq;
            mux = 2;
        } else {
            pRate = s_gpllFreq;
            mux = 0;
        }
        HAL_CRU_FracdivGetConfig(rate, pRate, &n, &m);
        HAL_CRU_ClkSetMux(muxSrc, mux);
        HAL_CRU_ClkSetFracDiv(divFrac, n, m);

        return HAL_OK;
    default:

        return HAL_INVAL;
    }
}

/** @} */
/********************* Public Function Definition ****************************/
static uint32_t HAL_CRU_ClkGetUartFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, freq;
    uint32_t uartTable[7] = { s_gpllFreq, s_cpllFreq, s_aupllFreq, PLL_INPUT_OSC_RATE, s_fracUart0Freq, s_fracUart1Freq, s_fracUart2Freq };

    s_fracUart0Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_0);
    s_fracUart1Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_1);
    s_fracUart2Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_2);

    switch (clockName) {
    case SCLK_UART0:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
    case SCLK_UART8:
    case SCLK_UART9:
    case SCLK_UART10:
    case SCLK_UART11:
    case CLK_UART1_SRC_TOP:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, uartTable, 7);
        break;
    case SCLK_UART1:
        return HAL_CRU_ClkGetUartFreq(CLK_UART1_SRC_TOP);
    default:

        return HAL_INVAL;
    }

    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

static HAL_Status HAL_CRU_ClkSetUartFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, mux = 0, div;
    uint32_t uartTable[7] = { s_gpllFreq, s_cpllFreq, s_aupllFreq, PLL_INPUT_OSC_RATE, s_fracUart0Freq, s_fracUart1Freq, s_fracUart2Freq };

    s_fracUart0Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_0);
    s_fracUart1Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_1);
    s_fracUart2Freq = HAL_CRU_ClkFracGetFreq(CLK_UART_FRAC_2);

    switch (clockName) {
    case SCLK_UART0:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
    case SCLK_UART8:
    case SCLK_UART9:
    case SCLK_UART10:
    case SCLK_UART11:
    case CLK_UART1_SRC_TOP:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, uartTable, 7, &pRate, 1);
        break;
    case SCLK_UART1:
        return HAL_CRU_ClkSetUartFreq(CLK_UART1_SRC_TOP, rate);
    default:

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

static uint32_t HAL_CRU_ClkGetAudioFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, freq;
    uint32_t audioTable0[8] = { PLL_INPUT_OSC_RATE, s_fracAudio0Freq, s_fracAudio1Freq, s_fracAudio2Freq, s_fracAudio3Freq, s_intAudio0Freq, s_intAudio1Freq, s_intAudio2Freq };
    uint32_t audioTable1[4] = { s_fracAudio0Freq, s_fracAudio1Freq, s_fracAudio2Freq, s_fracAudio3Freq };

    s_fracAudio0Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_0);
    s_fracAudio1Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_1);
    s_fracAudio2Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_2);
    s_fracAudio3Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_3);
    s_intAudio0Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_0);
    s_intAudio1Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_1);
    s_intAudio2Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_2);

    switch (clockName) {
    case CLK_PDM0_SRC_TOP:
    case CLK_PDM1:
    case MCLK_PDM0_SRC_TOP:
    case MCLK_PDM1:
    case MCLK_SAI0_8CH_SRC:
    case MCLK_SAI1_8CH_SRC:
    case MCLK_SAI2_2CH_SRC:
    case MCLK_SAI3_2CH_SRC:
    case MCLK_SAI4_2CH_SRC:
    case MCLK_SAI5_8CH_SRC:
    case MCLK_SAI6_8CH_SRC:
    case MCLK_SAI7_8CH_SRC:
    case MCLK_SAI8_8CH_SRC:
    case MCLK_SAI9_8CH_SRC:
    case MCLK_SPDIF_TX0:
    case MCLK_SPDIF_TX1:
    case MCLK_SPDIF_TX2:
    case MCLK_SPDIF_TX3:
    case MCLK_SPDIF_TX4:
    case MCLK_SPDIF_TX5:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, audioTable0, 8);
        break;
    case LCLK_ASRC_SRC_0:
    case LCLK_ASRC_SRC_1:
        pRate = HAL_CRU_MuxGetFreqArray(clkMux, audioTable1, 4);
        break;
    case CLK_ASRC_2CH_0:
    case CLK_ASRC_2CH_1:
    case CLK_ASRC_4CH_0:
    case CLK_ASRC_4CH_1:
    case MCLK_SPDIF_RX2:
        pRate = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_cpllFreq, s_aupllFreq);
        break;

    default:

        return HAL_INVAL;
    }
    if (clkDiv) {
        freq = pRate / (HAL_CRU_ClkGetDiv(clkDiv));
    } else {
        freq = pRate;
    }

    return freq;
}

static HAL_Status HAL_CRU_ClkSetAudioFreq(eCLOCK_Name clockName, uint32_t rate)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t pRate = s_gpllFreq, mux = 0, div;
    uint32_t audioTable0[8] = { PLL_INPUT_OSC_RATE, s_fracAudio0Freq, s_fracAudio1Freq, s_fracAudio2Freq, s_fracAudio3Freq, s_intAudio0Freq, s_intAudio1Freq, s_intAudio2Freq };
    uint32_t audioTable1[4] = { s_fracAudio0Freq, s_fracAudio1Freq, s_fracAudio2Freq, s_fracAudio3Freq };

    s_fracAudio0Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_0);
    s_fracAudio1Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_1);
    s_fracAudio2Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_2);
    s_fracAudio3Freq = HAL_CRU_ClkFracGetFreq(CLK_AUDIO_FRAC_3);
    s_intAudio0Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_0);
    s_intAudio1Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_1);
    s_intAudio2Freq = HAL_CRU_ClkGetFreq(CLK_AUDIO_INT_2);

    switch (clockName) {
    case CLK_PDM0_SRC_TOP:
    case CLK_PDM1:
    case MCLK_PDM0_SRC_TOP:
    case MCLK_PDM1:
    case MCLK_SAI0_8CH_SRC:
    case MCLK_SAI1_8CH_SRC:
    case MCLK_SAI2_2CH_SRC:
    case MCLK_SAI3_2CH_SRC:
    case MCLK_SAI4_2CH_SRC:
    case MCLK_SAI5_8CH_SRC:
    case MCLK_SAI6_8CH_SRC:
    case MCLK_SAI7_8CH_SRC:
    case MCLK_SAI8_8CH_SRC:
    case MCLK_SAI9_8CH_SRC:
    case MCLK_SPDIF_TX0:
    case MCLK_SPDIF_TX1:
    case MCLK_SPDIF_TX2:
    case MCLK_SPDIF_TX3:
    case MCLK_SPDIF_TX4:
    case MCLK_SPDIF_TX5:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, audioTable0, 8, &pRate, 1);
        break;
    case LCLK_ASRC_SRC_0:
    case LCLK_ASRC_SRC_1:
        mux = HAL_CRU_RoundFreqGetMuxArray(rate, audioTable1, 4, &pRate, 1);
        break;
    case CLK_ASRC_2CH_0:
    case CLK_ASRC_2CH_1:
    case CLK_ASRC_4CH_0:
    case CLK_ASRC_4CH_1:
    case MCLK_SPDIF_RX2:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_cpllFreq, s_aupllFreq, &pRate);
        break;

    default:

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

static uint32_t HAL_CRU_ClkGetVopFreq(eCLOCK_Name clockName)
{
    uint32_t mux = CLK_GET_MUX(clockName);
    uint32_t muxSrc, divSrc;
    uint32_t freq = HAL_INVAL;
    uint32_t muxVal;

    switch (clockName) {
    case DCLK_VP0:
        muxSrc = CLK_GET_MUX(DCLK_VP0_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP0_SRC);
        muxVal = HAL_CRU_ClkGetMux(mux);
        break;

    case DCLK_VP1:
        muxSrc = CLK_GET_MUX(DCLK_VP1_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP1_SRC);
        muxVal = HAL_CRU_ClkGetMux(mux);
        break;

    case DCLK_VP2:
        muxSrc = CLK_GET_MUX(DCLK_VP2_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP2_SRC);
        muxVal = HAL_CRU_ClkGetMux(mux);
        break;

    default:

        return HAL_INVAL;
    }

    if (muxVal == 0 && muxSrc && divSrc) {
        freq = HAL_CRU_MuxGetFreq4(muxSrc, s_gpllFreq, s_cpllFreq, s_vpllFreq, 0);
        freq /= HAL_CRU_ClkGetDiv(divSrc);
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 ") => (0x%08" PRIX32 "|0U): srcMux=%" PRId32 ", mux=%" PRId32 ", "
                "rate=%" PRId32 ", div=%" PRId32 ", prate0=%" PRId32 ", prate1=%" PRId32 ", prate2=%" PRId32 ", prate3=%" PRId32 "\n",
                __func__, muxSrc, divSrc, muxVal, HAL_CRU_ClkGetMux(muxSrc),
                HAL_CRU_ClkGetMux(mux), freq, HAL_CRU_ClkGetDiv(divSrc),
                s_gpllFreq, s_cpllFreq, s_vpllFreq, s_aupllFreq);

    return freq;
}

static uint32_t HAL_CRU_ClkSetVopFreq(eCLOCK_Name clockName, uint32_t rate)
{
    /* vop2Pll[3] can be set as PLL_AUPLL, if the product doesn't use audio */
    uint32_t vop2Pll[] = { 0, 0, PLL_VPLL, 0, 0 };
    uint32_t mux = CLK_GET_MUX(clockName);
    uint32_t muxSrc, divSrc;
    uint32_t maxDiv, pllFreq;
    uint32_t *pllTable = NULL;
    uint32_t pllTableCnt = 0;
    uint32_t curPll, i;
    int best = -1;

    switch (clockName) {
    case DCLK_VP0:
        muxSrc = CLK_GET_MUX(DCLK_VP0_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP0_SRC);
        break;

    case DCLK_VP1:
        muxSrc = CLK_GET_MUX(DCLK_VP1_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP1_SRC);
        break;

    case DCLK_VP2:
        muxSrc = CLK_GET_MUX(DCLK_VP2_SRC);
        divSrc = CLK_GET_DIV(DCLK_VP2_SRC);
        pllTable = vop2Pll;
        pllTableCnt = HAL_ARRAY_SIZE(vop2Pll);
        break;

    default:

        return HAL_INVAL;
    }

    maxDiv = CLK_DIV_GET_MAXDIV(divSrc);

    if (DIV_NO_REM(s_vpllFreq, rate, maxDiv)) {
        HAL_CRU_ClkSetDiv(divSrc, s_vpllFreq / rate);
        HAL_CRU_ClkSetMux(muxSrc, 2);
        if (muxSrc != mux) {
            HAL_CRU_ClkSetMux(mux, 0);
        }
    } else if (DIV_NO_REM(s_gpllFreq, rate, maxDiv)) {
        HAL_CRU_ClkSetDiv(divSrc, s_gpllFreq / rate);
        HAL_CRU_ClkSetMux(muxSrc, 0);
        if (muxSrc != mux) {
            HAL_CRU_ClkSetMux(mux, 0);
        }
    } else if (DIV_NO_REM(s_cpllFreq, rate, maxDiv)) {
        HAL_CRU_ClkSetDiv(divSrc, s_cpllFreq / rate);
        HAL_CRU_ClkSetMux(muxSrc, 1);
        if (muxSrc != mux) {
            HAL_CRU_ClkSetMux(mux, 0);
        }
    } else {
        curPll = HAL_CRU_ClkGetMux(muxSrc);
        for (i = 0; i < pllTableCnt; i++) {
            if (pllTable[i]) {
                best = i;
                if (pllTable[i] == curPll) {
                    break;
                }
            }
        }

        /* No PLL reserved for vop ? */
        if (best < 0) {
            best = 0;
            pllFreq = s_gpllFreq;
        } else {
            HAL_CRU_ClkSetFreq(pllTable[best], rate);
            pllFreq = HAL_CRU_ClkGetFreq(pllTable[best]);
        }

        HAL_CRU_ClkSetDiv(divSrc, HAL_DIV_ROUND_UP(pllFreq, rate));
        HAL_CRU_ClkSetMux(muxSrc, best);
        if (muxSrc != mux) {
            HAL_CRU_ClkSetMux(mux, 0);
        }
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 ") => (0x%08" PRIX32 "|0U): best=%d, "
                "rate=%" PRId32 ", pRate=%" PRId32 ", muxSrc=%" PRId32 ", divSrc=%" PRId32 ", mux=%" PRId32 ", maxdiv=%" PRId32 "\n",
                __func__, muxSrc, divSrc, mux, best, rate,
                rate * HAL_CRU_ClkGetDiv(divSrc), HAL_CRU_ClkGetMux(muxSrc),
                HAL_CRU_ClkGetDiv(divSrc), HAL_CRU_ClkGetMux(mux), maxDiv);

    return HAL_OK;
}

static uint32_t HAL_CRU_ClkGetOtherFreq(eCLOCK_Name clockName)
{
    switch (clockName) {
    case CLK_AUDIO_FRAC_0:
    case CLK_AUDIO_FRAC_1:
    case CLK_AUDIO_FRAC_2:
    case CLK_AUDIO_FRAC_3:
    case CLK_UART_FRAC_0:
    case CLK_UART_FRAC_1:
    case CLK_UART_FRAC_2:

        return HAL_CRU_ClkFracGetFreq(clockName);

    case CLK_PDM0_SRC_TOP:
    case CLK_PDM1:
    case LCLK_ASRC_SRC_0:
    case LCLK_ASRC_SRC_1:
    case MCLK_PDM0_SRC_TOP:
    case MCLK_PDM1:
    case MCLK_SAI0_8CH_SRC:
    case MCLK_SAI1_8CH_SRC:
    case MCLK_SAI2_2CH_SRC:
    case MCLK_SAI3_2CH_SRC:
    case MCLK_SAI4_2CH_SRC:
    case MCLK_SAI5_8CH_SRC:
    case MCLK_SAI6_8CH_SRC:
    case MCLK_SAI7_8CH_SRC:
    case MCLK_SAI8_8CH_SRC:
    case MCLK_SAI9_8CH_SRC:
    case CLK_ASRC_2CH_0:
    case CLK_ASRC_2CH_1:
    case CLK_ASRC_4CH_0:
    case CLK_ASRC_4CH_1:
    case MCLK_SPDIF_TX0:
    case MCLK_SPDIF_TX1:
    case MCLK_SPDIF_TX2:
    case MCLK_SPDIF_RX2:
    case MCLK_SPDIF_TX3:
    case MCLK_SPDIF_TX4:
    case MCLK_SPDIF_TX5:
        return HAL_CRU_ClkGetAudioFreq(clockName);
    case SCLK_UART0:
    case SCLK_UART1:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
    case SCLK_UART8:
    case SCLK_UART9:
    case SCLK_UART10:
    case SCLK_UART11:
    case CLK_UART1_SRC_TOP:
        return HAL_CRU_ClkGetUartFreq(clockName);
    case DCLK_VP0:
    case DCLK_VP1:
    case DCLK_VP2:

        return HAL_CRU_ClkGetVopFreq(clockName);
    default:
        break;
    }

    return HAL_INVAL;
}

static HAL_Status HAL_CRU_ClkSetOtherFreq(eCLOCK_Name clockName, uint32_t rate)
{
    switch (clockName) {
    case CLK_AUDIO_FRAC_0:
    case CLK_AUDIO_FRAC_1:
    case CLK_AUDIO_FRAC_2:
    case CLK_AUDIO_FRAC_3:
    case CLK_UART_FRAC_0:
    case CLK_UART_FRAC_1:
    case CLK_UART_FRAC_2:

        return HAL_CRU_ClkFracSetFreq(clockName, rate);

    case CLK_PDM0_SRC_TOP:
    case CLK_PDM1:
    case LCLK_ASRC_SRC_0:
    case LCLK_ASRC_SRC_1:
    case MCLK_PDM0_SRC_TOP:
    case MCLK_PDM1:
    case MCLK_SAI0_8CH_SRC:
    case MCLK_SAI1_8CH_SRC:
    case MCLK_SAI2_2CH_SRC:
    case MCLK_SAI3_2CH_SRC:
    case MCLK_SAI4_2CH_SRC:
    case MCLK_SAI5_8CH_SRC:
    case MCLK_SAI6_8CH_SRC:
    case MCLK_SAI7_8CH_SRC:
    case MCLK_SAI8_8CH_SRC:
    case MCLK_SAI9_8CH_SRC:
    case CLK_ASRC_2CH_0:
    case CLK_ASRC_2CH_1:
    case CLK_ASRC_4CH_0:
    case CLK_ASRC_4CH_1:
    case MCLK_SPDIF_TX0:
    case MCLK_SPDIF_TX1:
    case MCLK_SPDIF_TX2:
    case MCLK_SPDIF_RX2:
    case MCLK_SPDIF_TX3:
    case MCLK_SPDIF_TX4:
    case MCLK_SPDIF_TX5:
        return HAL_CRU_ClkSetAudioFreq(clockName, rate);
    case SCLK_UART0:
    case SCLK_UART1:
    case SCLK_UART2:
    case SCLK_UART3:
    case SCLK_UART4:
    case SCLK_UART5:
    case SCLK_UART6:
    case SCLK_UART7:
    case SCLK_UART8:
    case SCLK_UART9:
    case SCLK_UART10:
    case SCLK_UART11:
    case CLK_UART1_SRC_TOP:
        return HAL_CRU_ClkSetUartFreq(clockName, rate);

    case DCLK_VP0:
    case DCLK_VP1:
    case DCLK_VP2:

        return HAL_CRU_ClkSetVopFreq(clockName, rate);

    default:
        break;
    }

    return HAL_INVAL;
}

static void CRU_InitPlls(void)
{
    s_cpllFreq = HAL_CRU_GetPllFreq(&CPLL);
    s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);
    s_vpllFreq = HAL_CRU_GetPllFreq(&VPLL);
    s_aupllFreq = HAL_CRU_GetPllFreq(&AUPLL);
    s_ppllFreq = HAL_CRU_GetPllFreq(&PPLL);
    s_lpllFreq = HAL_CRU_GetPllFreq(&LPLL);
    s_bpllFreq = HAL_CRU_GetPllFreq(&BPLL);
    HAL_CRU_DBG("%s: cpll=%" PRId32 ", gpll=%" PRId32 ", vpll=%" PRId32 ", aupll=%" PRId32 ", ppll=%" PRId32 ", lpll=%" PRId32 ", bpll=%" PRId32 "\n",
                __func__, s_cpllFreq, s_gpllFreq, s_vpllFreq, s_aupllFreq, s_ppllFreq, s_lpllFreq, s_bpllFreq);
}

uint32_t HAL_CRU_ClkGetFreq(eCLOCK_Name clockName)
{
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t freq = 0;

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 ")\n", __func__, clkMux, clkDiv);
    HAL_CRU_DBG("%s: cpll=%" PRId32 ", gpll=%" PRId32 ", vpll=%" PRId32 ", aupll=%" PRId32 ", ppll=%" PRId32 "\n",
                __func__, s_cpllFreq, s_gpllFreq, s_vpllFreq, s_aupllFreq, s_ppllFreq);

    if (!s_cpllFreq) {
        CRU_InitPlls();
    }

    switch (clockName) {
    case PLL_LPLL:
        freq = HAL_CRU_GetPllFreq(&LPLL);
        s_lpllFreq = freq;

        return freq;

    case PLL_BPLL:
        freq = HAL_CRU_GetPllFreq(&BPLL);
        s_bpllFreq = freq;

        return freq;

    case PLL_CPLL:
        freq = HAL_CRU_GetPllFreq(&CPLL);
        s_cpllFreq = freq;

        return freq;

    case PLL_VPLL:
        freq = HAL_CRU_GetPllFreq(&VPLL);
        s_vpllFreq = freq;

        return freq;

    case PLL_AUPLL:
        freq = HAL_CRU_GetPllFreq(&AUPLL);
        s_aupllFreq = freq;

        return freq;

    case PLL_PPLL:
        freq = HAL_CRU_GetPllFreq(&PPLL);
        s_ppllFreq = freq;

        return freq;

    case PLL_GPLL:
        freq = HAL_CRU_GetPllFreq(&GPLL);
        s_gpllFreq = freq;

        return freq;

    case ACLK_SECURE_HIGH:
        freq = HAL_CRU_MuxGetFreq4(clkMux, s_gpllFreq, _MHZ(702), s_aupllFreq, s_bpllFreq / 4);
        break;
    case ACLK_TOP:
        freq = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_cpllFreq, s_aupllFreq);
        break;
    case HCLK_TOP:
    case HCLK_VO0VOP_CHANNEL:
    case HCLK_BUS_ROOT:
    case HCLK_CENTER_ROOT:
    case PCLK_CENTER_ROOT:
    case CLK_I2C0:
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_I2C3:
    case CLK_I2C4:
    case CLK_I2C5:
    case CLK_I2C6:
    case CLK_I2C7:
    case CLK_I2C8:
    case CLK_I2C9:
    case BCLK_EMMC:
        freq = HAL_CRU_MuxGetFreq4(clkMux, _MHZ(200), _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;
    case PCLK_BUS_ROOT:
        freq = HAL_CRU_MuxGetFreq3(clkMux, _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;

    case ACLK_VO0VOP_CHANNEL:
        freq = HAL_CRU_MuxGetFreq4(clkMux, s_gpllFreq, s_cpllFreq, 0, 0);
        break;
    case ACLK_BUS_ROOT:
    case ACLK_TOP_MID:
        freq = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, s_cpllFreq);
        break;

    case ACLK_CENTER_ROOT:
        freq = HAL_CRU_MuxGetFreq4(clkMux, s_gpllFreq, s_cpllFreq, _MHZ(702), s_aupllFreq);
        break;
    case ACLK_CENTER_LOW_ROOT:
        freq = HAL_CRU_MuxGetFreq4(clkMux, _MHZ(500), _MHZ(250), _MHZ(100), PLL_INPUT_OSC_RATE);
        break;
    case CLK_CAN0:
    case CLK_CAN1:
        freq = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE);
        break;
    case CLK_GMAC0_125M_SRC:
    case CLK_GMAC1_125M_SRC:
        freq = s_cpllFreq;
        break;

    case CCLK_SRC_EMMC:
    case CCLK_SRC_SDIO:
    case SCLK_FSPI_X2:
    case SCLK_FSPI1_X2:
    case CCLK_SRC_SDMMC0:
        freq = HAL_CRU_MuxGetFreq3(clkMux, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE);
        break;

    case ACLK_NVM_ROOT:
    case ACLK_UFS_ROOT:
    case ACLK_USB_ROOT:
        freq = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, s_cpllFreq);
        break;
    case DCLK_DECOM:
        freq = HAL_CRU_MuxGetFreq2(clkMux, s_gpllFreq, _MHZ(702));
        break;

    case CLK_SPI0:
    case CLK_SPI1:
    case CLK_SPI2:
    case CLK_SPI3:
    case CLK_SPI4:
        freq = HAL_CRU_MuxGetFreq4(clkMux, _MHZ(200), _MHZ(150), _MHZ(100), PLL_INPUT_OSC_RATE);
        break;

    case CLK_PWM1:
    case CLK_PWM2:
    case CLK_PMU1PWM:
        freq = HAL_CRU_FreqGetMux3(clkMux, _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;

    case CLK_AUDIO_INT_0:
        freq = s_gpllFreq;
        break;
    case CLK_AUDIO_INT_1:
        freq = s_cpllFreq;
        break;
    case CLK_AUDIO_INT_2:
        freq = s_aupllFreq;
        break;
    case CLK_SARADC:
        freq = HAL_CRU_FreqGetMux2(clkMux, s_gpllFreq, PLL_INPUT_OSC_RATE);
        break;

    case CLK_TSADC:
        freq = PLL_INPUT_OSC_RATE;
        break;
    default:

        return HAL_CRU_ClkGetOtherFreq(clockName);
    }

    if (!clkMux && !clkDiv) {
        return 0;
    }
    if (clkDiv) {
        freq /= (HAL_CRU_ClkGetDiv(clkDiv));
    }

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 "): freq: %" PRId32 "=%" PRId32 "/%" PRId32 "\n",
                __func__, clkMux, clkDiv, freq, freq * HAL_CRU_ClkGetDiv(clkDiv),
                HAL_CRU_ClkGetDiv(clkDiv));

    return freq;
}

HAL_Status HAL_CRU_ClkSetFreq(eCLOCK_Name clockName, uint32_t rate)
{
    HAL_Status error = HAL_OK;
    uint32_t clkMux = CLK_GET_MUX(clockName);
    uint32_t clkDiv = CLK_GET_DIV(clockName);
    uint32_t mux = 0, div = 1, pRate = 0;

    HAL_CRU_DBG("%s: (0x%08" PRIX32 "|0x%08" PRIX32 "): rate=%" PRId32 "\n", __func__, clkMux, clkDiv, rate);
    HAL_CRU_DBG("%s: cpll=%" PRId32 ", gpll=%" PRId32 ", v0pll=%" PRId32 ", aupll=%" PRId32 ", ppll=%" PRId32 "\n",
                __func__, s_cpllFreq, s_gpllFreq, s_vpllFreq, s_aupllFreq, s_ppllFreq);

    if (!s_cpllFreq) {
        CRU_InitPlls();
    }

    switch (clockName) {
    case PLL_LPLL:
        error = HAL_CRU_SetPllFreq(&LPLL, rate);
        s_lpllFreq = HAL_CRU_GetPllFreq(&LPLL);

        return error;

    case PLL_BPLL:
        error = HAL_CRU_SetPllFreq(&BPLL, rate);
        s_bpllFreq = HAL_CRU_GetPllFreq(&BPLL);

        return error;

    case PLL_CPLL:
        error = HAL_CRU_SetPllFreq(&CPLL, rate);
        s_cpllFreq = HAL_CRU_GetPllFreq(&CPLL);

        return error;

    case PLL_PPLL:
        error = HAL_CRU_SetPllFreq(&PPLL, rate);
        s_ppllFreq = HAL_CRU_GetPllFreq(&PPLL);

        return error;

    case PLL_GPLL:
        error = HAL_CRU_SetPllFreq(&GPLL, rate);
        s_gpllFreq = HAL_CRU_GetPllFreq(&GPLL);

        return error;

    case PLL_AUPLL:
        error = HAL_CRU_SetPllFreq(&AUPLL, rate);
        s_aupllFreq = HAL_CRU_GetPllFreq(&AUPLL);

        return error;

    case PLL_VPLL:
        error = HAL_CRU_SetPllFreq(&VPLL, rate);
        s_vpllFreq = HAL_CRU_GetPllFreq(&VPLL);

        return error;
    case ACLK_SECURE_HIGH:
        mux = HAL_CRU_RoundFreqGetMux4(rate, s_gpllFreq, _MHZ(702), s_aupllFreq, s_bpllFreq / 4, &pRate);
        break;
    case ACLK_TOP:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_gpllFreq, s_aupllFreq, &pRate);
        break;

    case HCLK_TOP:
    case HCLK_VO0VOP_CHANNEL:
    case HCLK_BUS_ROOT:
    case HCLK_CENTER_ROOT:
    case PCLK_CENTER_ROOT:
    case CLK_I2C0:
    case CLK_I2C1:
    case CLK_I2C2:
    case CLK_I2C3:
    case CLK_I2C4:
    case CLK_I2C5:
    case CLK_I2C6:
    case CLK_I2C7:
    case CLK_I2C8:
    case CLK_I2C9:
    case BCLK_EMMC:
        mux = HAL_CRU_FreqGetMux4(rate, _MHZ(200), _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;
    case PCLK_BUS_ROOT:
        mux = HAL_CRU_FreqGetMux3(rate, _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;

    case ACLK_VO0VOP_CHANNEL:
        mux = HAL_CRU_RoundFreqGetMux4(rate, s_gpllFreq, s_cpllFreq, 0, 0, &pRate);
        break;
    case ACLK_BUS_ROOT:
    case ACLK_TOP_MID:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, s_cpllFreq, &pRate);
        break;

    case ACLK_CENTER_ROOT:
        mux = HAL_CRU_RoundFreqGetMux4(rate, s_gpllFreq, s_cpllFreq, _MHZ(702), s_aupllFreq, &pRate);
        break;
    case ACLK_CENTER_LOW_ROOT:
        mux = HAL_CRU_FreqGetMux4(rate, _MHZ(500), _MHZ(250), _MHZ(100), PLL_INPUT_OSC_RATE);
        break;
    case CLK_CAN0:
    case CLK_CAN1:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;
    case CLK_GMAC0_125M_SRC:
    case CLK_GMAC1_125M_SRC:
        pRate = s_cpllFreq;
        break;

    case CCLK_SRC_EMMC:
    case CCLK_SRC_SDIO:
    case SCLK_FSPI_X2:
    case SCLK_FSPI1_X2:
    case CCLK_SRC_SDMMC0:
        mux = HAL_CRU_RoundFreqGetMux3(rate, s_gpllFreq, s_cpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;

    case ACLK_NVM_ROOT:
    case ACLK_UFS_ROOT:
    case ACLK_USB_ROOT:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, s_cpllFreq, &pRate);
        break;
    case DCLK_DECOM:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, _MHZ(702), &pRate);
        break;

    case CLK_SPI0:
    case CLK_SPI1:
    case CLK_SPI2:
    case CLK_SPI3:
    case CLK_SPI4:
        mux = HAL_CRU_FreqGetMux4(rate, _MHZ(200), _MHZ(150), _MHZ(100), PLL_INPUT_OSC_RATE);
        break;

    case CLK_PWM1:
    case CLK_PWM2:
    case CLK_PMU1PWM:
        mux = HAL_CRU_FreqGetMux3(rate, _MHZ(100), _MHZ(50), PLL_INPUT_OSC_RATE);
        break;

    case CLK_AUDIO_INT_0:
        pRate = s_gpllFreq;
        break;
    case CLK_AUDIO_INT_1:
        pRate = s_cpllFreq;
        break;
    case CLK_AUDIO_INT_2:
        pRate = s_aupllFreq;
        break;
    case CLK_SARADC:
        mux = HAL_CRU_RoundFreqGetMux2(rate, s_gpllFreq, PLL_INPUT_OSC_RATE, &pRate);
        break;

    case CLK_TSADC:
        pRate = PLL_INPUT_OSC_RATE;
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
        mask = CRU_GLB_RST_CON_WDT_TRIG_GLBRST_SEL_MASK | CRU_GLB_RST_CON_OSC_CHK_TRIG_GLBRST_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_WDT_TRIG_GLBRST_SEL_SHIFT) | (1 << CRU_GLB_RST_CON_OSC_CHK_TRIG_GLBRST_EN_SHIFT);
        break;
    case GLB_RST_SND_WDT0:
        mask = CRU_GLB_RST_CON_WDT_TRIG_GLBRST_SEL_MASK | CRU_GLB_RST_CON_OSC_CHK_TRIG_GLBRST_EN_MASK;
        val = (1 << CRU_GLB_RST_CON_OSC_CHK_TRIG_GLBRST_EN_SHIFT);
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

#endif /* SOC_RK3588 && HAL_CRU_MODULE_ENABLED */
