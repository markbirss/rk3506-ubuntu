/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"
#include "hal_math.h"

#ifdef HAL_INNO_MIPI_DPHY_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup INNO_MIPI_DPHY
 *  @{
 */

/** @defgroup INNO_MIPI_DPHY_How_To_Use How To Use
 *  @{

 The INNO MIPI DPHY driver can be used as follows:

   - Enable and Disable different components:
      - Use HAL_INNO_MIPI_DPHY_BgpdEnable() to enable the BGPD (Band Gap Power Down) in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_DaPwrokEnable() to enable the DA PWRON signal in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_PllLdoEnable() to enable the PLL LDO in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_LaneEnable() to enable a specific lane in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_Reset() to reset the MIPI DPHY.

   - Initialize and configure the MIPI DPHY:
      - Use HAL_INNO_MIPI_DPHY_TimingInit() to initialize the DPHY timing parameters like lane data rate, lane number, and the SoC type.
      - Use HAL_INNO_MIPI_DPHY_SetPll() to configure the PLL settings with feedback divider, pre-divider, and SoC type.

   - Disable specific components:
      - Use HAL_INNO_MIPI_DPHY_LaneDisable() to disable a specific lane in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_PllLdoDisable() to disable the PLL LDO in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_DaPwrokDisable() to disable the DA PWRON signal in the MIPI DPHY.
      - Use HAL_INNO_MIPI_DPHY_BgpdDisable() to disable the BGPD in the MIPI DPHY.

 @} */

/** @defgroup INNO_MIPI_DPHY_Private_Definition Private Definition
 *  @{
 */
#define UPDATE(v, h, l)    (((v) << (l)) & HAL_GENMASK((h), (l)))
#define TIMING_NS(x, freq) (((x) * (HAL_DIV_ROUND_CLOSEST(1000000000, freq))))

#define FBDIV_HI(x) UPDATE(x, 5, 5)
#define FBDIV_LO(x) UPDATE(x, 7, 0)
#define PREDIV(x)   UPDATE(x, 4, 0)

#define T_LPX(x)         UPDATE(x, 5, 0)
#define T_HS_PREPARE(x)  UPDATE(x, 6, 0)
#define T_HS_ZERO(x)     UPDATE(x, 5, 0)
#define T_HS_TRAIL(x)    UPDATE(x, 6, 0)
#define T_HS_EXIT(x)     UPDATE(x, 4, 0)
#define T_CLK_POST(x)    UPDATE(x, 3, 0)
#define T_CLK_POST_HI(x) UPDATE(x, 7, 6)
#define T_WAKUP_H(x)     UPDATE(x, 1, 0)
#define T_WAKUP_L(x)     UPDATE(x, 7, 0)
#define T_CLK_PRE(x)     UPDATE(x, 3, 0)
#define T_TA_GO(x)       UPDATE(x, 5, 0)
#define T_TA_SURE(x)     UPDATE(x, 5, 0)
#define T_TA_WAIT(x)     UPDATE(x, 5, 0)

#define FIXED_PARAM(_freq, _lpx, _prepare, _clkZero, _dataZero, _trail) \
{                                                                       \
    .maxFreq = _freq,                                                   \
    .lpx = _lpx,                                                        \
    .hsPrepare = _prepare,                                              \
    .clkLane = {                                                        \
        .hsZero = _clkZero,                                             \
    },                                                                  \
    .dataLane = {                                                       \
        .hsZero = _dataZero,                                            \
    },                                                                  \
    .hsTrail = _trail,                                                  \
}

/********************* Private Structure Definition **************************/

typedef enum {
    CLOCK_LANE,
    DATA_LANE_0,
    DATA_LANE_1,
    DATA_LANE_2,
    DATA_LANE_3,
} eINNO_MIPI_DPHY_laneType;

struct MIPI_DPHY_TIMING {
    unsigned int clkMiss;
    unsigned int clkPost;
    unsigned int clkPre;
    unsigned int clkPrepare;
    unsigned int clkSettle;
    unsigned int clkTermen;
    unsigned int clkTrail;
    unsigned int clkZero;
    unsigned int dTermen;
    unsigned int eot;
    unsigned int hsExit;
    unsigned int hsPrepare;
    unsigned int hsZero;
    unsigned int hsSettle;
    unsigned int hsSkip;
    unsigned int hsTrail;
    unsigned int init;
    unsigned int lpx;
    unsigned int taGet;
    unsigned int taGo;
    unsigned int taSure;
    unsigned int wakeUp;
};

struct INNO_MIPI_DPHY_TIMING {
    uint8_t lpx;
    uint8_t hsPrepare;
    uint8_t hsZero;
    uint8_t hsTrail;
    uint8_t hsExit;
    uint8_t clkPost;
    uint8_t wakupH;
    uint8_t wakupL;
    uint8_t clkPre;
    uint8_t taGo;
    uint8_t taSure;
    uint8_t taWait;
};

struct FIXED_PARAM {
    unsigned int maxFreq; // Max frequency in MHz
    uint8_t lpx;
    uint8_t hsPrepare;
    struct {
        uint8_t hsZero;
    } clkLane;
    struct {
        uint8_t hsZero;
    } dataLane;
    uint8_t hsTrail;
};

static const struct FIXED_PARAM FIXED_PARAM_TABLE[] = {
    FIXED_PARAM(110, 0x0, 0x20, 0x16, 0x02, 0x22),
    FIXED_PARAM(150, 0x0, 0x06, 0x16, 0x03, 0x45),
    FIXED_PARAM(200, 0x0, 0x18, 0x17, 0x04, 0x0b),
    FIXED_PARAM(250, 0x0, 0x05, 0x17, 0x05, 0x16),
    FIXED_PARAM(300, 0x0, 0x51, 0x18, 0x06, 0x2c),
    FIXED_PARAM(400, 0x0, 0x64, 0x19, 0x07, 0x33),
    FIXED_PARAM(500, 0x0, 0x20, 0x1b, 0x07, 0x4e),
    FIXED_PARAM(600, 0x0, 0x6a, 0x1d, 0x08, 0x3a),
    FIXED_PARAM(700, 0x0, 0x3e, 0x1e, 0x08, 0x6a),
    FIXED_PARAM(800, 0x0, 0x21, 0x1f, 0x09, 0x29),
    FIXED_PARAM(1000, 0x0, 0x09, 0x20, 0x09, 0x27)
};

static const struct FIXED_PARAM RK1808_FIXED_PARAM_TABLE[] = {
    FIXED_PARAM(110, 0x02, 0x7f, 0x16, 0x02, 0x02),
    FIXED_PARAM(150, 0x02, 0x7f, 0x16, 0x03, 0x02),
    FIXED_PARAM(200, 0x02, 0x7f, 0x17, 0x04, 0x02),
    FIXED_PARAM(250, 0x02, 0x7f, 0x17, 0x05, 0x04),
    FIXED_PARAM(300, 0x02, 0x7f, 0x18, 0x06, 0x04),
    FIXED_PARAM(400, 0x03, 0x7e, 0x19, 0x07, 0x04),
    FIXED_PARAM(500, 0x03, 0x7c, 0x1b, 0x07, 0x08),
    FIXED_PARAM(600, 0x03, 0x70, 0x1d, 0x08, 0x10),
    FIXED_PARAM(700, 0x05, 0x40, 0x1e, 0x08, 0x30),
    FIXED_PARAM(800, 0x05, 0x02, 0x1f, 0x09, 0x30),
    FIXED_PARAM(1000, 0x05, 0x08, 0x20, 0x09, 0x30),
    FIXED_PARAM(1400, 0x09, 0x03, 0x32, 0x14, 0x0f),
    FIXED_PARAM(1600, 0x0d, 0x42, 0x36, 0x0e, 0x0f),
    FIXED_PARAM(1800, 0x0e, 0x47, 0x7a, 0x0e, 0x0f),
    FIXED_PARAM(2000, 0x11, 0x64, 0x7a, 0x0e, 0x0b),
};

/********************* Private Function Definition ***************************/

static void INNO_MIPI_DPHY_TimingGetDefault(struct MIPI_DPHY_TIMING *timing,
                                            unsigned long period)
{
    /* Global Operation Timing Parameters */
    timing->clkMiss = 0;
    timing->clkPost = 70 + 52 * period;
    timing->clkPre = 8 * period;
    timing->clkPrepare = 65;
    timing->clkSettle = 95;
    timing->clkTermen = 0;
    timing->clkTrail = 80;
    timing->clkZero = 260;
    timing->dTermen = 0;
    timing->eot = 0;
    timing->hsExit = 120;
    timing->hsPrepare = 65 + 4 * period;
    timing->hsZero = 145 + 6 * period;
    timing->hsSettle = 85 + 6 * period;
    timing->hsSkip = 40;
    timing->hsTrail = HAL_MAX(8 * period, 60 + 4 * period);
    timing->init = 100000;
    timing->lpx = 60;
    timing->taGet = 5 * timing->lpx;
    timing->taGo = 4 * timing->lpx;
    timing->taSure = 2 * timing->lpx;
    timing->wakeUp = 1000000;
}

static void INNO_MIPI_DPHY_GetFixedParam(struct INNO_MIPI_DPHY_TIMING *t,
                                         unsigned int freq,
                                         eINNO_MIPI_DPHY_socType socType,
                                         eINNO_MIPI_DPHY_laneType laneType)
{
    const struct FIXED_PARAM *param, *paramTable;
    int i, paramNum;

    if (socType == RK1808_INNO_MIPI_DPHY) {
        paramTable = RK1808_FIXED_PARAM_TABLE;
        paramNum = HAL_ARRAY_SIZE(RK1808_FIXED_PARAM_TABLE);
    } else {
        paramTable = FIXED_PARAM_TABLE;
        paramNum = HAL_ARRAY_SIZE(FIXED_PARAM_TABLE);
    }

    for (i = 0; i < paramNum; i++) {
        if (freq <= paramTable[i].maxFreq) {
            break;
        }
    }

    if (i == paramNum) {
        --i;
    }

    param = &paramTable[i];

    if (laneType == CLOCK_LANE) {
        t->hsZero = param->clkLane.hsZero;
    } else {
        t->hsZero = param->dataLane.hsZero;
    }

    t->hsPrepare = param->hsPrepare;
    t->hsTrail = param->hsTrail;

    if (socType == RK1808_INNO_MIPI_DPHY) {
        t->lpx = param->lpx;
    }
}

static void INNO_MIPI_DPHY_TimingUpdate(struct MIPI_TX_PHY_REG *pReg,
                                        eINNO_MIPI_DPHY_laneType laneType,
                                        struct INNO_MIPI_DPHY_TIMING *t)
{
    uint32_t v;

    if (laneType == DATA_LANE_0) {
        v = T_HS_PREPARE(t->hsPrepare);
        WRITE_REG(pReg->DPHY_DATA_LANE0_THSPREPARE, v);

        v = T_HS_ZERO(t->hsZero);
        WRITE_REG(pReg->DPHY_DATA_LANE0_THSZERO, v);

        v = T_HS_TRAIL(t->hsTrail);
        WRITE_REG(pReg->DPHY_DATA_LANE0_THSTRAIL, v);

        v = T_HS_EXIT(t->hsExit);
        WRITE_REG(pReg->DPHY_DATA_LANE0_THSEXIT, v);
    }

    if (laneType == DATA_LANE_1) {
        v = T_HS_PREPARE(t->hsPrepare);
        WRITE_REG(pReg->DPHY_DATA_LANE1_THSPREPARE, v);

        v = T_HS_ZERO(t->hsZero);
        WRITE_REG(pReg->DPHY_DATA_LANE1_THSZERO, v);

        v = T_HS_TRAIL(t->hsTrail);
        WRITE_REG(pReg->DPHY_DATA_LANE1_THSTRAIL, v);

        v = T_HS_EXIT(t->hsExit);
        WRITE_REG(pReg->DPHY_DATA_LANE1_THSEXIT, v);
    }

#ifdef MIPI_TX_PHY_DPHY_DATA_LANE2_THSPREPARE_OFFSET
    if (laneType == DATA_LANE_2) {
        v = T_HS_PREPARE(t->hsPrepare);
        WRITE_REG(pReg->DPHY_DATA_LANE2_THSPREPARE, v);

        v = T_HS_ZERO(t->hsZero);
        WRITE_REG(pReg->DPHY_DATA_LANE2_THSZERO, v);

        v = T_HS_TRAIL(t->hsTrail);
        WRITE_REG(pReg->DPHY_DATA_LANE2_THSTRAIL, v);

        v = T_HS_EXIT(t->hsExit);
        WRITE_REG(pReg->DPHY_DATA_LANE2_THSEXIT, v);
    }

    if (laneType == DATA_LANE_3) {
        v = T_HS_PREPARE(t->hsPrepare);
        WRITE_REG(pReg->DPHY_DATA_LANE3_THSPREPARE, v);

        v = T_HS_ZERO(t->hsZero);
        WRITE_REG(pReg->DPHY_DATA_LANE3_THSZERO, v);

        v = T_HS_TRAIL(t->hsTrail);
        WRITE_REG(pReg->DPHY_DATA_LANE3_THSTRAIL, v);

        v = T_HS_EXIT(t->hsExit);
        WRITE_REG(pReg->DPHY_DATA_LANE3_THSEXIT, v);
    }
#endif

    if (laneType == CLOCK_LANE) {
        v = T_HS_PREPARE(t->hsPrepare);
        WRITE_REG(pReg->DPHY_CLK_LANE_THSPREPARE, v);

        v = T_HS_ZERO(t->hsZero);
        WRITE_REG(pReg->DPHY_CLK_LANE_THSZERO, v);

        v = T_HS_TRAIL(t->hsTrail);
        WRITE_REG(pReg->DPHY_CLK_LANE_THSTRAIL, v);

        v = T_HS_EXIT(t->hsExit);
        WRITE_REG(pReg->DPHY_CLK_LANE_THSEXIT, v);

        v = T_CLK_POST(t->clkPost);
        WRITE_REG(pReg->DPHY_CLK_LANE_TPOST, v);

        v = T_CLK_POST_HI(t->clkPost >> 4);
        WRITE_REG(pReg->DPHY_CLK_LANE_Ttago, v);

        v = T_CLK_PRE(t->clkPre);
        WRITE_REG(pReg->DPHY_CLK_LANE_TPRE, v);

        v = T_WAKUP_H(t->wakupH);
        WRITE_REG(pReg->DPHY_CLK_LANE_LPDT_EN, v);

        v = T_WAKUP_L(t->wakupL);
        WRITE_REG(pReg->RESERVED0134, v);

        v = T_LPX(t->lpx);
        WRITE_REG(pReg->DPHY_CLK_LANE_TLPX, v);

        v = T_TA_GO(t->taGo);
        WRITE_REG(pReg->DPHY_CLK_LANE_Ttago, v);

        v = T_TA_SURE(t->taSure);
        WRITE_REG(pReg->DPHY_CLK_LANE_TTASURE, v);

        v = T_TA_WAIT(t->taWait);
        WRITE_REG(pReg->DPHY_CLK_LANE_TTAWAIT, v);
    }
}

static void INNO_MIPI_DPHY_LaneTimingInit(struct MIPI_TX_PHY_REG *pReg, eINNO_MIPI_DPHY_socType socType, unsigned int laneMbps, eINNO_MIPI_DPHY_laneType laneType)
{
    struct MIPI_DPHY_TIMING timing;
    struct INNO_MIPI_DPHY_TIMING data;
    uint32_t txByteClk, txClkEsc, UI;
    unsigned int escClkDiv;

    memset(&timing, 0, sizeof(timing));
    memset(&data, 0, sizeof(data));

    txByteClk = laneMbps * 1000000 / 8;
    escClkDiv = HAL_DIV_ROUND_UP(txByteClk, 20000000);
    txClkEsc = txByteClk / escClkDiv;
    UI = HAL_DIV_ROUND_CLOSEST(1000, laneMbps);

    INNO_MIPI_DPHY_TimingGetDefault(&timing, UI);
    INNO_MIPI_DPHY_GetFixedParam(&data, laneMbps, socType, laneType);

    /*
     * TtxByteClk * val >= Ths-exit
     * TtxByteClk * val >= Tclk-post
     * TtxByteClk * val >= Tclk-pre
     * TtxByteClk * (2 + val) >= Tlpx
     */
    data.hsExit = HAL_DIV_ROUND_UP(timing.hsExit * txByteClk, 1000000000);
    data.clkPost = HAL_DIV_ROUND_UP(timing.clkPost * txByteClk, 1000000000);
    data.clkPre = HAL_DIV_ROUND_UP(timing.clkPre * txByteClk, 1000000000);
    data.wakupH = 0x3;
    data.wakupL = 0xff;

    /*
     * TtxClkEsc * val >= Tta-go
     * TtxClkEsc * val >= Tta-sure
     * TtxClkEsc * val >= Tta-wait
     */
    data.taGo = HAL_DIV_ROUND_UP(timing.taGo * txClkEsc, 1000000000);
    data.taSure = HAL_DIV_ROUND_UP(timing.taSure * txClkEsc, 1000000000);
    data.taWait = HAL_DIV_ROUND_UP(timing.taGet * txClkEsc, 1000000000);

    INNO_MIPI_DPHY_TimingUpdate(pReg, laneType, &data);

    HAL_DBG("hs-exit=%lu, clk-post=%lu, clk-pre=%lu, lpx=%lu\n",
            TIMING_NS(data.hsExit, txByteClk),
            TIMING_NS(data.clkPost, txByteClk),
            TIMING_NS(data.clkPre, txByteClk),
            TIMING_NS(data.lpx + 2, txByteClk));
    HAL_DBG("ta-go=%lu, ta-sure=%lu, ta-wait=%lu\n",
            TIMING_NS(data.taGo, txClkEsc),
            TIMING_NS(data.taSure, txClkEsc),
            TIMING_NS(data.taWait, txClkEsc));
}

/********************* Public Function Definition ****************************/

/**
 * @brief  DPHY Bgpd Enable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_BgpdEnable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    CLEAR_BIT(pReg->DPHY_CTRL_LANE_ENABLE, MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_BANDGAP_POWER_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Power Enable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_DaPwrokEnable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    CLEAR_BIT(pReg->DPHY_CTRL_LANE_ENABLE, MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_POWER_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Pll Ldo Enable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_PllLdoEnable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    WRITE_REG_MASK_WE(pReg->DPHY_POWER_CTRL, MIPI_TX_PHY_DPHY_POWER_CTRL_PLLPD_MASK |
                      MIPI_TX_PHY_DPHY_POWER_CTRL_LDOPD_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  DPHY Lane Enable.
 * @param  pReg: DPHY reg base.
 * @param  laneNum: lane number.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_LaneEnable(struct MIPI_TX_PHY_REG *pReg, uint32_t laneNum)
{
    uint32_t m = 0;

    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));
    HAL_ASSERT(laneNum != 0);

    switch (laneNum) {
    case 4:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_3_MASK;
    /* fallthrough */
    case 3:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_2_MASK;
    /* fallthrough */
    case 2:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_1_MASK;
    /* fallthrough */
    default:
    case 1:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_0_MASK | MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_CK_MASK;
        break;
    }

    SET_BIT(pReg->DPHY_CTRL_LANE_ENABLE, m);

    return HAL_OK;
}

/**
 * @brief  DPHY Reset.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_Reset(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    /* Reset analog */
    SET_BIT(pReg->DPHY_POWER_CTRL, MIPI_TX_PHY_DPHY_POWER_CTRL_SYNCRST_MASK);
    HAL_DelayUs(1);
    CLEAR_BIT(pReg->DPHY_POWER_CTRL, MIPI_TX_PHY_DPHY_POWER_CTRL_SYNCRST_MASK);
    /* Reset digital */
    CLEAR_BIT(pReg->DPHY_DIGITAL_CTRL, MIPI_TX_PHY_DPHY_DIGITAL_CTRL_DIG_RSTN_MASK);
    HAL_DelayUs(1);
    SET_BIT(pReg->DPHY_DIGITAL_CTRL, MIPI_TX_PHY_DPHY_DIGITAL_CTRL_DIG_RSTN_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Tining Init.
 * @param  pReg: DPHY reg base.
 * @param  laneMbps: lane frequency in Mbps.
 * @param  laneNum: lane number.
 * @param  socType: DPHY Soc Type.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_TimingInit(struct MIPI_TX_PHY_REG *pReg, unsigned int laneMbps, uint32_t laneNum, eINNO_MIPI_DPHY_socType socType)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));
    HAL_ASSERT(laneMbps != 0);
    HAL_ASSERT(laneNum != 0);
    HAL_ASSERT(socType != 0);

    switch (laneNum) {
    case 4:
        INNO_MIPI_DPHY_LaneTimingInit(pReg, socType, laneMbps, DATA_LANE_3);
    /* fallthrough */
    case 3:
        INNO_MIPI_DPHY_LaneTimingInit(pReg, socType, laneMbps, DATA_LANE_2);
    /* fallthrough */
    case 2:
        INNO_MIPI_DPHY_LaneTimingInit(pReg, socType, laneMbps, DATA_LANE_1);
    /* fallthrough */
    case 1:
    default:
        INNO_MIPI_DPHY_LaneTimingInit(pReg, socType, laneMbps, DATA_LANE_0);
        INNO_MIPI_DPHY_LaneTimingInit(pReg, socType, laneMbps, CLOCK_LANE);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  DPHY Lane Disable.
 * @param  pReg: DPHY reg base.
 * @param  laneNum: lane number.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_LaneDisable(struct MIPI_TX_PHY_REG *pReg, uint32_t laneNum)
{
    uint32_t m = 0;

    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));
    HAL_ASSERT(laneNum != 0);

    switch (laneNum) {
    case 4:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_3_MASK;
    /* fallthrough */
    case 3:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_2_MASK;
    /* fallthrough */
    case 2:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_1_MASK;
    /* fallthrough */
    default:
    case 1:
        m |= MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_0_MASK | MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_LANE_EN_CK_MASK;
        break;
    }

    CLEAR_BIT(pReg->DPHY_CTRL_LANE_ENABLE, m);

    return HAL_OK;
}

/**
 * @brief  DPHY Pll Ldo Disable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_PllLdoDisable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    WRITE_REG_MASK_WE(pReg->DPHY_POWER_CTRL, MIPI_TX_PHY_DPHY_POWER_CTRL_PLLPD_MASK | MIPI_TX_PHY_DPHY_POWER_CTRL_LDOPD_MASK,
                      MIPI_TX_PHY_DPHY_POWER_CTRL_PLLPD_MASK | MIPI_TX_PHY_DPHY_POWER_CTRL_PLLPD_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Power Disable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_DaPwrokDisable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    SET_BIT(pReg->DPHY_CTRL_LANE_ENABLE, MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_POWER_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Bgpd Disable.
 * @param  pReg: DPHY reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_BgpdDisable(struct MIPI_TX_PHY_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));

    SET_BIT(pReg->DPHY_CTRL_LANE_ENABLE, MIPI_TX_PHY_DPHY_CTRL_LANE_ENABLE_BANDGAP_POWER_MASK);

    return HAL_OK;
}

/**
 * @brief  DPHY Set Pll.
 * @param  pReg: DPHY reg base.
 * @param  fbDiv: Feedback Divider.
 * @param  preDiv: Pre-divider.
 * @param  socType: DPHY Soc Type.
 * @return HAL_Status.
 */
HAL_Status HAL_INNO_MIPI_DPHY_SetPll(struct MIPI_TX_PHY_REG *pReg, uint16_t fbDiv, uint8_t preDiv, eINNO_MIPI_DPHY_socType socType)
{
    uint32_t v;

    HAL_ASSERT(IS_MIPI_TX_PHY_INSTANCE(pReg));
    HAL_ASSERT(fbDiv != 0);
    HAL_ASSERT(preDiv != 0);
    HAL_ASSERT(socType != 0);

    v = FBDIV_HI(fbDiv >> 8) | PREDIV(preDiv);
    WRITE_REG(pReg->DPHY_PREDIV_CTRL, v);

    v = FBDIV_LO(fbDiv);
    WRITE_REG(pReg->DPHY_FBDIV_CTRL, v);

    if (socType == RK3506_INNO_MIPI_DPHY) {
        SET_BIT(pReg->DPHY_PRE_EMPHASIS, MIPI_TX_PHY_DPHY_PRE_EMPHASIS_LANE1_PRE_EMPHASIS_EN_MASK);
        SET_BIT(pReg->DPHY_PRE_EMPHASIS, MIPI_TX_PHY_DPHY_PRE_EMPHASIS_LANE0_PRE_EMPHASIS_EN_MASK);
        SET_BIT(pReg->DPHY_PRE_EMPHASIS, MIPI_TX_PHY_DPHY_PRE_EMPHASIS_CLK_LANE_PRE_EMPHASIS_EN_MASK);

        WRITE_REG(pReg->DPHY_CLK_PRE_EMPHASIS_RANGE, MIPI_TX_PHY_DPHY_CLK_PRE_EMPHASIS_RANGE_CLK_LANE_PRE_EMPHASIS_RANGE_MASK);
        WRITE_REG(pReg->DPHY_LANE0_PRE_EMPHASIS_RANGE, MIPI_TX_PHY_DPHY_LANE0_PRE_EMPHASIS_RANGE_LANE0_PRE_EMPHASIS_RANGE_MASK);
        WRITE_REG(pReg->DPHY_LANE1_PRE_EMPHASIS_RANGE, MIPI_TX_PHY_DPHY_LANE1_PRE_EMPHASIS_RANGE_LANE1_PRE_EMPHASIS_RANGE_MASK);
        WRITE_REG(pReg->DPHY_CLK_LANE_VOD, MIPI_TX_PHY_DPHY_CLK_LANE_VOD_CLOCK_LANE_VOD_MASK);
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_INNO_MIPI_DPHY_MODULE_ENABLED */
