/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_DSMC_HOST_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DSMC_HOST
 *  @{
 */

/** @defgroup DSMC_HOST_How_To_Use How To Use
 *  @{

 The DSMC_HOST driver can be used as follows:

 - Invoke HAL_DSMC_HOST_Init() to initialize DSMC_HOST.
 - Invoke HAL_DSMC_HOST_DeInit() to deinitialize DSMC_HOST.

 @} */

/** @defgroup DSMC_HOST_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/* INT_EN */
#define INT_EN(cs) (0x1 << (cs))

/* INT_STATUS */
#define INT_STATUS(cs)      (0x1 << (cs))
#define INT_STATUS_MASK(cs) (0x1 << (cs))

/* INT_MASK */
#define INT_MASK(cs)   (0x1 << (cs))
#define INT_UNMASK(cs) (0x0 << (cs))

/* DMA_EN */
#define DMA_REQ_MASK(cs) (0x1 << (cs))
#define DMA_REQ_EN(cs)   (0x1 << (cs))
#define DMA_REQ_DIS(cs)  (0x0 << (cs))

/* DSMC_DMA_MUX */
#define DMA_REQ_MUX_MASK(req) (0x3 << ((req) * 4))
#define DMA_REQ_MUX(req, n)   (((n) & 0x3) << ((req) * 4))

/* VDMC */
#define VDMC_LATENCY_VARIABLE 0
#define VDMC_LATENCY_FIXED    1

/* MCR */
#define MCR_WRAPSIZE_32_CLK 1
#define MCR_WRAPSIZE_8_CLK  2
#define MCR_WRAPSIZE_16_CLK 3

/* BDRTCR */
#define BDRTCR_WR_BDR_XFER_EN 1
#define BDRTCR_RD_BDR_XFER_EN 1

/* MRGTCR */
#define MRGTCR_READ_WRITE_MERGE_EN 0x3

#define MTR_CFG(RCSHI, WCSHI, RCSS, WCSS, RCSH, WCSH, RLTCY, WLTCY) \
    ((((RCSHI) << DSMC_MTR0_RCSHI_SHIFT) & DSMC_MTR0_RCSHI_MASK) |  \
     (((WCSHI) << DSMC_MTR0_WCSHI_SHIFT) & DSMC_MTR0_WCSHI_MASK) |  \
     (((RCSS) << DSMC_MTR0_RCSS_SHIFT) & DSMC_MTR0_RCSS_MASK) |     \
     (((WCSS) << DSMC_MTR0_WCSS_SHIFT) & DSMC_MTR0_WCSS_MASK) |     \
     (((RCSH) << DSMC_MTR0_RCSH_SHIFT) & DSMC_MTR0_RCSH_MASK) |     \
     (((WCSH) << DSMC_MTR0_WCSH_SHIFT) & DSMC_MTR0_WCSH_MASK) |     \
     (((RLTCY) << DSMC_MTR0_RLTCY_SHIFT) & DSMC_MTR0_RLTCY_MASK) |  \
     (((WLTCY) << DSMC_MTR0_WLTCY_SHIFT) & DSMC_MTR0_WLTCY_MASK))

#define APM_PSRAM_LATENCY_FIXED    0x1
#define APM_PSRAM_LATENCY_VARIABLE 0x0

#define DSMC_DLL_EN 0x1

#define HYPER_PSRAM_IR0        (0x00)
#define HYPER_PSRAM_IR1        (0x02)
#define HYPER_PSRAM_CR0        (0x1000)
#define HYPER_PSRAM_CR1        (0x1002)
#define XCCELA_PSRAM_MR(n)     (2 * (n))
#define XCCELA_PSRAM_MR_GET(n) (((n) >> 8) & 0xff)
#define XCCELA_PSRAM_MR_SET(n) (((n) & 0xff) << 8)
/* device id bit mask */
#define HYPERBUS_DEV_ID_MASK   (0xf)
#define IR0_ROW_COUNT_128MBIT  (0xd)
#define IR0_ROW_COUNT_SHIFT    (0x8)
#define IR0_ROW_COUNT_MASK     (0x1f)
#define IR0_COL_COUNT_SHIFT    (0x4)
#define IR0_COL_COUNT_MASK     (0xf)
#define IR1_DEV_IO_WIDTH_SHIFT (0)
#define IR1_DEV_IO_WIDTH_MASK  (0xf)
#define IR1_DEV_IO_WIDTH_X16   (0x9)

#define CR0_INITIAL_LATENCY_SHIFT                 4
#define CR0_INITIAL_LATENCY_MASK                  0xf
#define CR0_FIXED_LATENCY_ENABLE_SHIFT            3
#define CR0_FIXED_LATENCY_ENABLE_MASK             0x1
#define CR0_FIXED_LATENCY_ENABLE_VARIABLE_LATENCY 0x0
#define CR0_FIXED_LATENCY_ENABLE_FIXED_LATENCY    0x1

#define CR0_BURST_LENGTH_SHIFT  0
#define CR0_BURST_LENGTH_MASK   0x3
#define CR0_BURST_LENGTH_64_CLK 0x0
#define CR0_BURST_LENGTH_32_CLK 0x1
#define CR0_BURST_LENGTH_8_CLK  0x2
#define CR0_BURST_LENGTH_16_CLK 0x3

#define CR1_CLOCK_TYPE_SHIFT      6
#define CR1_CLOCK_TYPE_MASK       0x1
#define CR1_CLOCK_TYPE_SINGLE_CLK 0x1
#define CR1_CLOCK_TYPE_DIFF_CLK   0x0

#define XCCELA_DEV_ID_MASK (0x1f)

#define XCCELA_MR0_RL_SHIFT         (2)
#define XCCELA_MR0_RL_MASK          (0x7)
#define XCCELA_MR0_RL_TYPE_SHIFT    (5)
#define XCCELA_MR0_RL_TYPE_MASK     (0x1)
#define XCCELA_MR0_RL_TYPE_FIXED    (0x1)
#define XCCELA_MR0_RL_TYPE_VARIABLE (0x0)

#define XCCELA_MR2_DEV_DENSITY_MASK (0x7)

#define XCCELA_MR3_RBXEN_MASK  (1)
#define XCCELA_MR3_RBXEN_SHIFT (7)

#define XCCELA_MR4_WL_SHIFT (5)
#define XCCELA_MR4_WL_MASK  (0x7)

#define XCCELA_MR8_IO_TYPE_SHIFT (6)
#define XCCELA_MR8_IO_TYPE_MASK  (0x1)
#define XCCELA_MR8_IO_TYPE_X16   (0x1)
#define XCCELA_MR8_IO_TYPE_X8    (0x0)
#define XCCELA_MR8_RBX_EN_SHIFT  (3)
#define XCCELA_MR8_RBX_EN_MASK   (0x1)
#define XCCELA_MR8_RBX_EN        (0x1)
#define XCCELA_MR8_BL_SHIFT      (0)
#define XCCELA_MR8_BL_MASK       (0x7)
#define XCCELA_MR8_BL_32_CLK     (0x2)
#define XCCELA_MR8_BL_16_CLK     (0x1)
#define XCCELA_MR8_BL_8_CLK      (0x0)

#define PSRAM_SIZE_32MBYTE (0x02000000)
#define PSRAM_SIZE_16MBYTE (0x01000000)
#define PSRAM_SIZE_8MBYTE  (0x00800000)
#define PSRAM_SIZE_4MBYTE  (0x00400000)

/* TCSM/TCEM */
#define DSMC_DEV_TCSM_4U   (4000)
#define DSMC_DEV_TCSM_1U   (1000)
#define DSMC_DEC_TCEM_2_5U (2500)
#define DSMC_DEC_TCEM_3U   (3000)
#define DSMC_DEC_TCEM_0_5U (500)

/* LBC_SLAVE_CMN register */
#define CMN_CON(n)            (0x4 * (n))
#define CMN_STATUS            (0x80)
#define RGN_CMN_CON(rgn, com) (0x100 + 0x100 * (rgn) + 0x4 * (com))
#define DBG_STATUS(n)         (0x900 + 0x4 * (n))

/* LBC_SLAVE_CSR register */
#define APP_CON(n)             (0x4 * (n))
#define APP_H2S_INT_STA        (0x80)
#define APP_H2S_INT_STA_EN     (0x84)
#define APP_H2S_INT_STA_SIG_EN (0x88)
#define LBC_CON(n)             (0x100 + 0x4 * (n))
#define LBC_S2H_INT_STA        (0x180)
#define LBC_S2H_INT_STA_EN     (0x184)
#define LBC_S2H_INT_STA_SIG_EN (0x188)
#define AXI_WR_ADDR_BASE       (0x800)
#define AXI_RD_ADDR_BASE       (0x804)
#define DBG_STA(n)             (0x900 + 0x4 * (n))

/* LBC_S2H_INT_STA */
#define LBC_S2H_INT_STA_SHIFT (0)
#define LBC_S2H_INT_STA_MASK  (0xFFFF)
/* LBC_S2H_INT_STA_EN */
#define LBC_S2H_INT_STA_EN_SHIFT (0)
#define LBC_S2H_INT_STA_EN_MASK  (0xFFFF)
/* LBC_S2H_INT_STA_SIG_EN */
#define LBC_S2H_INT_STA_SIG_EN_SHIFT (0)
#define LBC_S2H_INT_STA_SIG_EN_MASK  (0xFFFF)

/* LBC_SLAVE_CMN_CMN_CON0 */
#define CA_CYC_16BIT               (0)
#define CA_CYC_32BIT               (1)
#define CA_CYC_SHIFT               (0)
#define CA_CYC_MASK                (0x1)
#define WR_LATENCY_CYC_SHIFT       (4)
#define WR_LATENCY_CYC_MASK        (0x7)
#define RD_LATENCY_CYC_SHIFT       (8)
#define RD_LATENCY_CYC_MASK        (0x7)
#define WR_DATA_CYC_EXTENDED_SHIFT (11)
#define WR_DATA_CYC_EXTENDED_MASK  (0x1)

/* LBC_SLAVE_CMN_CMN_CON3 */
#define DATA_WIDTH_SHIFT    (0)
#define DATA_WIDTH_MASK     (0x1)
#define RDYN_GEN_CTRL_SHIFT (4)
#define RDYN_GEN_CTRL_MASK  (0x1)

/********************* Private Structure Definition **************************/

enum {
    CYPRESS   = 0x1,
    ISSI      = 0x3,
    WINBOND   = 0x6,
    APM_PSRAM = 0xd,
};

struct dsmc_psram {
    uint16_t id;
    uint16_t protcl;
    uint32_t mtrTiming;
};

/* DSMC psram chip features */
static const struct dsmc_psram DSMC_PsramInfo[] = {
    { APM_PSRAM, OPI_XCCELA_PSRAM, MTR_CFG(2, 2, 0, 0, 0, 0, 0, 0) },
    { WINBOND, HYPERBUS_PSRAM, MTR_CFG(2, 2, 0, 0, 0, 0, 2, 2) },
    { CYPRESS, HYPERBUS_PSRAM, MTR_CFG(2, 2, 0, 0, 0, 0, 1, 1) },
    { ISSI, HYPERBUS_PSRAM, MTR_CFG(2, 2, 0, 0, 0, 0, 1, 1) },
};

/********************* Private Function Definition ***************************/
static inline void xccelaWriteMR(struct DSMC_MAP *map,
                                 uint32_t mrNum, uint8_t val)
{
    *((volatile uint16_t *)(map->phys + XCCELA_PSRAM_MR(mrNum))) = XCCELA_PSRAM_MR_SET(val);
}

static inline uint8_t xccelaReadMR(struct DSMC_MAP *map, uint32_t mrNum)
{
    return XCCELA_PSRAM_MR_GET(*((volatile uint16_t *)(map->phys + XCCELA_PSRAM_MR(mrNum))));
}

static inline void hyperWriteMR(struct DSMC_MAP *map,
                                uint32_t mrNum, uint16_t val)
{
    *((volatile uint16_t *)(map->phys + mrNum)) = val;
}

static inline uint16_t hyperReadMR(struct DSMC_MAP *map, uint32_t mrNum)
{
    return *((volatile uint16_t *)(map->phys + mrNum));
}

static inline void lbWriteReg(struct DSMC_MAP *map,
                              uint32_t offset, uint32_t val)
{
    volatile uint32_t *reg_ptr;

    reg_ptr = (volatile uint32_t *)(map->phys + offset);
    WRITE_REG(*reg_ptr, val);
}

static inline uint32_t lbReadReg(struct DSMC_MAP *map, uint32_t offset)

{
    volatile uint32_t *reg_ptr;

    reg_ptr = (volatile uint32_t *)(map->phys + offset);

    return READ_REG(*reg_ptr);
}

static int DSMC_HOST_FindAttrRgn(struct DSMC_CONFIG_CS *cfg, uint32_t attribute)
{
    int region;

    for (region = 0; region < DSMC_LB_MAX_RGN; region++) {
        if ((cfg->slvRgn[region].status) &&
            (cfg->slvRgn[region].attribute == attribute)) {
            return region;
        }
    }

    return -1;
}

static uint32_t DSMC_HOST_GetCsReg(struct DSMC_REG *pReg, uint32_t cs)
{
    uint32_t csReg = 0;

    switch (cs) {
    case 0:
        csReg = (uint32_t)&pReg->VDMC0;
        break;
    case 1:
        csReg = (uint32_t)&pReg->VDMC1;
        break;
    case 2:
        csReg = (uint32_t)&pReg->VDMC2;
        break;
    case 3:
        csReg = (uint32_t)&pReg->VDMC3;
        break;
    default:
        HAL_DBG("%s, %d, error cs\n", __func__, __LINE__);
    }

    return csReg;
}

static uint32_t DSMC_HOST_cap2devSize(uint32_t cap)
{
    uint32_t mask = 0x80000000;
    int i;

    for (i = 31; i >= 0; i--) {
        if (cap & mask) {
            return i;
        }
        mask >>= 1;
    }

    return 0;
}

static int DSMC_HOST_PsramIdDetect(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t tmp, i;
    int ret = -1;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
    struct DSMC_CONFIG_CS *cfg = &dsmcHostDev->ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    tmp = READ_REG(pRegCs->MCR);

    /* config to CR space */
    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_IOWIDTH_MASK |
               DSMC_MCR0_CRT_MASK,
               (MCR_IOWIDTH_X8 << DSMC_MCR0_IOWIDTH_SHIFT) |
               (MCR_CRT_CR_SPACE << DSMC_MCR0_CRT_SHIFT));

    if (cfg->protcl == OPI_XCCELA_PSRAM) {
        uint8_t mid;

        /* reset AP memory psram */
        MODIFY_REG(pRegCs->VDMC,
                   DSMC_VDMC0_RESET_CMD_MODE_MASK,
                   0x1 << DSMC_VDMC0_RESET_CMD_MODE_SHIFT);
        /* write mr any value to trigger xccela psram reset */
        xccelaWriteMR(regionMap, 0, 0x0);
        HAL_CPUDelayUs(200);
        MODIFY_REG(pRegCs->VDMC,
                   DSMC_VDMC0_RESET_CMD_MODE_MASK,
                   0x0 << DSMC_VDMC0_RESET_CMD_MODE_SHIFT);

        mid = xccelaReadMR(regionMap, 1);
        mid &= XCCELA_DEV_ID_MASK;

        if (mid == APM_PSRAM) {
            ret = 0;
        }
    } else {
        /* hyper psram get ID */
        uint16_t mid;

        mid = hyperReadMR(regionMap, HYPER_PSRAM_IR0);
        mid &= HYPERBUS_DEV_ID_MASK;
        for (i = 1; i < HAL_ARRAY_SIZE(DSMC_PsramInfo); i++) {
            if (mid == DSMC_PsramInfo[i].id) {
                ret = 0;
                break;
            }
        }
    }

    /* config to memory space */
    WRITE_REG(pRegCs->MCR, tmp);

    return ret;
}

static void DSMC_HOST_PsramBwDetect(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t tmp, col;
    uint16_t ir0_ir1;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
    struct DSMC_CONFIG_CS *cfg = &dsmcHostDev->ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    if (cfg->protcl == OPI_XCCELA_PSRAM) {
        col = 10;
        if (dsmcHostDev->cap >= PSRAM_SIZE_16MBYTE) {
            cfg->ioWidth = MCR_IOWIDTH_X16;
        } else {
            cfg->ioWidth = MCR_IOWIDTH_X8;
        }
    } else {
        tmp = READ_REG(pRegCs->MCR);
        /* config to CR space */
        MODIFY_REG(pRegCs->MCR,
                   DSMC_MCR0_IOWIDTH_MASK |
                   DSMC_MCR0_CRT_MASK,
                   (MCR_IOWIDTH_X8 << DSMC_MCR0_IOWIDTH_SHIFT) |
                   (MCR_CRT_CR_SPACE << DSMC_MCR0_CRT_SHIFT));

        /* hyper psram get IR0 */
        ir0_ir1 = hyperReadMR(regionMap, HYPER_PSRAM_IR0);
        col = ((ir0_ir1 >> IR0_COL_COUNT_SHIFT) & IR0_COL_COUNT_MASK) + 1;

        ir0_ir1 = hyperReadMR(regionMap, HYPER_PSRAM_IR1);
        if ((ir0_ir1 & IR1_DEV_IO_WIDTH_MASK) == IR1_DEV_IO_WIDTH_X16) {
            cfg->ioWidth = MCR_IOWIDTH_X16;
        } else {
            cfg->ioWidth = MCR_IOWIDTH_X8;
        }

        /* config to memory space */
        WRITE_REG(pRegCs->MCR, tmp);
    }
    cfg->col = col;
}

static int DSMC_HOST_PsramDectect(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t i = 0;
    int ret = -1;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    /* axi read do not response error */
    MODIFY_REG(pReg->AXICTL,
               DSMC_AXICTL_RD_NO_ERR_MASK,
               0x1 << DSMC_AXICTL_RD_NO_ERR_SHIFT);

    for (i = 0; i < HAL_ARRAY_SIZE(DSMC_PsramInfo); i++) {
        MODIFY_REG(pRegCs->VDMC,
                   DSMC_VDMC0_MID_MASK |
                   DSMC_VDMC0_PROTOCOL_MASK,
                   ((DSMC_PsramInfo[i].id << DSMC_VDMC0_MID_SHIFT) &
                    DSMC_VDMC0_MID_MASK) |
                   ((DSMC_PsramInfo[i].protcl << DSMC_VDMC0_PROTOCOL_SHIFT) &
                    DSMC_VDMC0_PROTOCOL_MASK));
        WRITE_REG(pRegCs->MTR, DSMC_PsramInfo[i].mtrTiming);

        cfg->mid = DSMC_PsramInfo[i].id;
        cfg->protcl = DSMC_PsramInfo[i].protcl;
        cfg->mtrTiming = DSMC_PsramInfo[i].mtrTiming;
        if (!DSMC_HOST_PsramIdDetect(host, cs)) {
            HAL_SYSLOG("DSMC_HOST: The cs%" PRIu32 " %s PSRAM ID: 0x%x\n", cs,
                       (cfg->protcl == OPI_XCCELA_PSRAM) ? "XCCELA" : "HYPER",
                       DSMC_PsramInfo[i].id);
            ret = 0;
            break;
        }
    }
    if (i == HAL_ARRAY_SIZE(DSMC_PsramInfo)) {
        HAL_SYSLOG("DSMC_HOST: Unknown PSRAM device!\n");
        ret = -1;
    } else {
        DSMC_HOST_PsramBwDetect(host, cs);
    }

    /* recovery axi read response */
    MODIFY_REG(pReg->AXICTL,
               DSMC_AXICTL_RD_NO_ERR_MASK,
               (0x0 << DSMC_AXICTL_RD_NO_ERR_SHIFT));

    return ret;
}

static uint32_t DSMC_HOST_LtcyCalc(uint32_t latency)
{
    if ((latency >= 5) && (latency <= 10)) {
        return (latency - 5);
    } else {
        return (latency + 0xb);
    }
}

static int DSMC_HOST_LbCtrllerCfg(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t value = 0, i;
    const volatile uint32_t *regAddr;
    struct REGIONS_CONFIG *slvRgn;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    WRITE_REG(pReg->CLK_MD, host->dsmcHostDev.clkMode);
    WRITE_REG(pRegCs->MTR,
              MTR_CFG(cfg->rcshi, cfg->wcshi, cfg->rcss, cfg->wcss,
                      cfg->rcsh, cfg->wcsh,
                      DSMC_HOST_LtcyCalc(cfg->rdLatency),
                      DSMC_HOST_LtcyCalc(cfg->wrLatency)));
    WRITE_REG(pRegCs->SLV_RGN_DIV, cfg->rgnNum / 2);

    for (i = 0; i < DSMC_LB_MAX_RGN; i++) {
        slvRgn = &cfg->slvRgn[i];
        if (!slvRgn->status) {
            continue;
        }

        if (slvRgn->dummyClkNum >= 2) {
            value = (0x1 << RGNX_ATTR_DUM_CLK_EN_SHIFT) |
                    (0x1 << RGNX_ATTR_DUM_CLK_NUM_SHIFT);
        } else if (slvRgn->dummyClkNum >= 1) {
            value = (0x1 << RGNX_ATTR_DUM_CLK_EN_SHIFT) |
                    (0x0 << RGNX_ATTR_DUM_CLK_NUM_SHIFT);
        } else {
            value = 0x0 << RGNX_ATTR_DUM_CLK_EN_SHIFT;
        }
        regAddr = &pRegCs->RGN0_ATTR + i;
        WRITE_REG(*regAddr,
                  value |
                  ((slvRgn->attribute << DSMC_RGN0_ATTR0_RGN0_ATTR_SHIFT) &
                   DSMC_RGN0_ATTR0_RGN0_ATTR_MASK) |
                  ((slvRgn->cs0Ctrl << DSMC_RGN0_ATTR0_RGN0_CTRL_SHIFT) &
                   DSMC_RGN0_ATTR0_RGN0_CTRL_MASK) |
                  ((slvRgn->cs0BeCtrled << DSMC_RGN0_ATTR0_RGN0_BE_CTRLED_SHIFT) &
                   DSMC_RGN0_ATTR0_RGN0_BE_CTRLED_MASK) |
                  ((RGNX_ATTR_32BIT_ADDR_WIDTH << DSMC_RGN0_ATTR0_RGN0_ADDR_WIDTH_SHIFT) &
                   DSMC_RGN0_ATTR0_RGN0_ADDR_WIDTH_MASK));
    }
    /* clear and enable interrupt */
    SET_BIT(pReg->INT_STATUS, INT_STATUS(cfg->intEn));
    SET_BIT(pReg->INT_EN, INT_EN(cfg->intEn));

    if (host->dsmcHostDev.dmaReqMuxOffset && (cs < 2)) {
        regAddr = &pReg->VER + host->dsmcHostDev.dmaReqMuxOffset / sizeof(uint32_t);
        MODIFY_REG(*regAddr,
                   DMA_REQ_MUX_MASK(cs),
                   DMA_REQ_MUX(cs, cfg->intEn) & DMA_REQ_MUX_MASK(cs));
    }

    return 0;
}

static int DSMC_HOST_LbCmnRgnConfig(struct HAL_DSMC_HOST *host,
                                    struct REGIONS_CONFIG *slvRgn,
                                    uint32_t rgn, uint32_t cs)
{
    uint32_t tmp;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_CONFIG_CS *cfg = &dsmcHostDev->ChipSelCfg[cs];
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];

    tmp = lbReadReg(regionMap, RGN_CMN_CON(rgn, 0));
    if (slvRgn->dummyClkNum == 0) {
        tmp &= ~(WR_DATA_CYC_EXTENDED_MASK << WR_DATA_CYC_EXTENDED_SHIFT);
    } else if (slvRgn->dummyClkNum == 1) {
        tmp |= slvRgn->dummyClkNum << WR_DATA_CYC_EXTENDED_SHIFT;
    } else {
        HAL_DBG_ERR("DSMC_HOST: lb slave: dummy clk too large\n");

        return -1;
    }
    tmp &= ~(RD_LATENCY_CYC_MASK << RD_LATENCY_CYC_SHIFT);
    if ((cfg->rdLatency == 1) || (cfg->rdLatency == 2)) {
        tmp |= cfg->rdLatency << RD_LATENCY_CYC_SHIFT;
    } else {
        HAL_DBG_ERR("DSMC_HOST: lb slave: read latency value error\n");

        return -1;
    }
    tmp &= ~(WR_LATENCY_CYC_MASK << WR_LATENCY_CYC_SHIFT);
    if ((cfg->wrLatency == 1) || (cfg->wrLatency == 2)) {
        tmp |= cfg->wrLatency << WR_LATENCY_CYC_SHIFT;
    } else {
        HAL_DBG_ERR("DSMC_HOST: lb slave: write latency value error\n");

        return -1;
    }
    tmp &= ~(CA_CYC_MASK << CA_CYC_SHIFT);
    if (slvRgn->caAddrWidth == RGNX_ATTR_32BIT_ADDR_WIDTH) {
        tmp |= CA_CYC_32BIT << CA_CYC_SHIFT;
    } else {
        tmp |= CA_CYC_16BIT << CA_CYC_SHIFT;
    }

    lbWriteReg(regionMap, RGN_CMN_CON(rgn, 0), tmp);

    return 0;
}

static int DSMC_HOST_LbSlaveCmnConfig(struct HAL_DSMC_HOST *host,
                                      struct REGIONS_CONFIG *slvRgn, uint32_t cs)
{
    uint32_t tmp;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
    struct DSMC_CONFIG_CS *cfg = &dsmcHostDev->ChipSelCfg[cs];

    tmp = lbReadReg(regionMap, CMN_CON(3));
    tmp |= 0x1 << RDYN_GEN_CTRL_SHIFT;
    tmp &= ~(DATA_WIDTH_MASK << DATA_WIDTH_SHIFT);
    tmp |= cfg->ioWidth << DATA_WIDTH_SHIFT;
    lbWriteReg(regionMap, CMN_CON(3), tmp);

    tmp = lbReadReg(regionMap, CMN_CON(0));
    if (slvRgn->dummyClkNum == 0) {
        tmp &= ~(WR_DATA_CYC_EXTENDED_MASK << WR_DATA_CYC_EXTENDED_SHIFT);
    } else if (slvRgn->dummyClkNum == 1) {
        tmp |= slvRgn->dummyClkNum << WR_DATA_CYC_EXTENDED_SHIFT;
    } else {
        HAL_DBG_ERR("DSMC_HOST: lb slave: dummy clk too large\n");

        return -1;
    }

    tmp &= ~(CA_CYC_MASK << CA_CYC_SHIFT);
    if (slvRgn->caAddrWidth == RGNX_ATTR_32BIT_ADDR_WIDTH) {
        tmp |= CA_CYC_32BIT << CA_CYC_SHIFT;
    } else {
        tmp |= CA_CYC_16BIT << CA_CYC_SHIFT;
    }

    lbWriteReg(regionMap, CMN_CON(0), tmp);

    return 0;
}

static int DSMC_HOST_LbCmnConfig(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t tmp, i;
    volatile uint32_t *regAddr;
    struct DSMC_CS_REG *pRegCs;
    struct REGIONS_CONFIG *slvRgn;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_CONFIG_CS *cfg = &dsmcHostDev->ChipSelCfg[cs];
    int ret = 0;

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    tmp = READ_REG(pRegCs->MCR);
    /* config to CR space */
    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_IOWIDTH_MASK |
               DSMC_MCR0_CRT_MASK,
               (MCR_IOWIDTH_X8 << DSMC_MCR0_IOWIDTH_SHIFT) |
               (MCR_CRT_CR_SPACE << DSMC_MCR0_CRT_SHIFT));

    for (i = 0; i < DSMC_LB_MAX_RGN; i++) {
        slvRgn = &cfg->slvRgn[i];
        if (!slvRgn->status) {
            continue;
        }
        ret = DSMC_HOST_LbCmnRgnConfig(host, slvRgn, i, cs);
        if (ret) {
            break;
        }
    }

    slvRgn = &cfg->slvRgn[0];
    ret = DSMC_HOST_LbSlaveCmnConfig(host, slvRgn, cs);

    /* config to memory space */
    WRITE_REG(pRegCs->MCR, tmp);

    for (i = 0; i < DSMC_LB_MAX_RGN; i++) {
        slvRgn = &cfg->slvRgn[i];
        if (!slvRgn->status) {
            continue;
        }

        regAddr = &pRegCs->RGN0_ATTR + i;
        MODIFY_REG(*regAddr,
                   DSMC_RGN0_ATTR0_RGN0_ADDR_WIDTH_MASK,
                   (slvRgn->caAddrWidth << DSMC_RGN0_ATTR0_RGN0_ADDR_WIDTH_SHIFT) &
                   DSMC_RGN0_ATTR0_RGN0_ADDR_WIDTH_MASK);
    }

    return ret;
}

static void DSMC_HOST_LbCsrConfig(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t mcrTmp, rgnAttrTmp;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    mcrTmp = READ_REG(pRegCs->MCR);
    rgnAttrTmp = READ_REG(pRegCs->RGN0_ATTR);

    /* config to slave CSR space */
    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_CRT_MASK,
               MCR_CRT_MEM_SPACE << DSMC_MCR0_CRT_SHIFT);
    MODIFY_REG(pRegCs->RGN0_ATTR,
               DSMC_RGN0_ATTR0_RGN0_ATTR_MASK,
               RGNX_ATTR_REG << DSMC_RGN0_ATTR0_RGN0_ATTR_SHIFT);

    /* enable all s2h interrupt */
    lbWriteReg(regionMap, LBC_S2H_INT_STA_EN, 0xffffffff);

    lbWriteReg(regionMap, LBC_S2H_INT_STA_SIG_EN, 0xffffffff);

    /* clear all s2h interrupt */
    lbWriteReg(regionMap, LBC_S2H_INT_STA, LBC_S2H_INT_STA_MASK << LBC_S2H_INT_STA_SHIFT);

    /* config to normal memory space */
    WRITE_REG(pRegCs->MCR, mcrTmp);
    WRITE_REG(pRegCs->RGN0_ATTR, rgnAttrTmp);
}

static void DSMC_HOST_CfgLatency(uint32_t rd_ltcy, uint32_t wr_ltcy,
                                 struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    rd_ltcy = DSMC_HOST_LtcyCalc(rd_ltcy);
    wr_ltcy = DSMC_HOST_LtcyCalc(wr_ltcy);

    MODIFY_REG(pRegCs->MTR,
               DSMC_MTR0_RLTCY_MASK | DSMC_MTR0_WLTCY_MASK,
               ((rd_ltcy << DSMC_MTR0_RLTCY_SHIFT) & DSMC_MTR0_RLTCY_MASK) |
               ((wr_ltcy << DSMC_MTR0_WLTCY_SHIFT) & DSMC_MTR0_WLTCY_MASK));
}

static int DSMC_HOST_PsramCfg(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t latency, mcr, tmp;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    mcr = READ_REG(pRegCs->MCR);
    /* config to CR space */
    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_IOWIDTH_MASK | DSMC_MCR0_CRT_MASK,
               (MCR_IOWIDTH_X8 << DSMC_MCR0_IOWIDTH_SHIFT) |
               (MCR_CRT_CR_SPACE << DSMC_MCR0_CRT_SHIFT));
    if (cfg->protcl == OPI_XCCELA_PSRAM) {
        /* Xccela psram init */
        uint8_t mr_tmp, rbxen;

        mr_tmp = xccelaReadMR(regionMap, 0);
        tmp = cfg->rdLatency - 3;
        mr_tmp = (mr_tmp & (~(XCCELA_MR0_RL_MASK << XCCELA_MR0_RL_SHIFT))) |
                 (tmp << XCCELA_MR0_RL_SHIFT);
        mr_tmp |= XCCELA_MR0_RL_TYPE_VARIABLE << XCCELA_MR0_RL_TYPE_SHIFT;
        xccelaWriteMR(regionMap, 0, mr_tmp);

        mr_tmp = xccelaReadMR(regionMap, 4);
        latency = cfg->wrLatency;
        if (latency == 3) {
            tmp = 0;
        } else if (latency == 5) {
            tmp = 2;
        } else if (latency == 7) {
            tmp = 1;
        } else {
            tmp = latency;
        }

        mr_tmp = (mr_tmp & (~(XCCELA_MR4_WL_MASK << XCCELA_MR4_WL_SHIFT))) |
                 (tmp << XCCELA_MR4_WL_SHIFT);

        xccelaWriteMR(regionMap, 4, mr_tmp);

        DSMC_HOST_CfgLatency(cfg->rdLatency, cfg->wrLatency, host, cs);

        mr_tmp = xccelaReadMR(regionMap, 3);
        if ((mr_tmp >> XCCELA_MR3_RBXEN_SHIFT) & XCCELA_MR3_RBXEN_MASK) {
            rbxen = 1;
            cfg->rdBdrXferEn = 0;
        } else {
            rbxen = 0;
        }

        mr_tmp = xccelaReadMR(regionMap, 8);

        if (cfg->ioWidth == MCR_IOWIDTH_X16) {
            mr_tmp |= XCCELA_MR8_IO_TYPE_X16 << XCCELA_MR8_IO_TYPE_SHIFT;
        } else {
            mr_tmp &= (~(XCCELA_MR8_IO_TYPE_MASK << XCCELA_MR8_IO_TYPE_SHIFT));
            mr_tmp |= XCCELA_MR8_IO_TYPE_X8 << XCCELA_MR8_IO_TYPE_SHIFT;
        }
        mr_tmp &= (~(XCCELA_MR8_BL_MASK << XCCELA_MR8_BL_SHIFT));
        if (cfg->wrapSize == MCR_WRAPSIZE_8_CLK) {
            mr_tmp |= (XCCELA_MR8_BL_8_CLK << XCCELA_MR8_BL_SHIFT);
        } else if (cfg->wrapSize == MCR_WRAPSIZE_16_CLK) {
            mr_tmp |= (XCCELA_MR8_BL_16_CLK << XCCELA_MR8_BL_SHIFT);
        } else if (cfg->wrapSize == MCR_WRAPSIZE_32_CLK) {
            mr_tmp |= (XCCELA_MR8_BL_32_CLK << XCCELA_MR8_BL_SHIFT);
        }

        mr_tmp |= rbxen << XCCELA_MR8_RBX_EN_SHIFT;

        xccelaWriteMR(regionMap, 8, mr_tmp);
    } else {
        /* Hyper psram init */
        uint16_t cr_tmp;

        cr_tmp = hyperReadMR(regionMap, HYPER_PSRAM_IR0);
        if (((cr_tmp >> IR0_ROW_COUNT_SHIFT) & IR0_ROW_COUNT_MASK) ==
            IR0_ROW_COUNT_128MBIT) {
            cfg->rdBdrXferEn = 1;
            cfg->wrBdrXferEn = 1;
        } else {
            cfg->rdBdrXferEn = 0;
            cfg->wrBdrXferEn = 0;
        }

        cr_tmp = hyperReadMR(regionMap, HYPER_PSRAM_CR0);

        latency = cfg->wrLatency;
        if (latency == 3) {
            tmp = 0xe;
        } else if (latency == 4) {
            tmp = 0xf;
        } else {
            tmp = latency - 5;
        }
        cr_tmp = (cr_tmp & (~(CR0_INITIAL_LATENCY_MASK << CR0_INITIAL_LATENCY_SHIFT))) |
                 (tmp << CR0_INITIAL_LATENCY_SHIFT);

        cr_tmp = (cr_tmp & (~(CR0_BURST_LENGTH_MASK << CR0_BURST_LENGTH_SHIFT)));

        if (cfg->wrapSize == MCR_WRAPSIZE_8_CLK) {
            cr_tmp |= (CR0_BURST_LENGTH_8_CLK << CR0_BURST_LENGTH_SHIFT);
        } else if (cfg->wrapSize == MCR_WRAPSIZE_16_CLK) {
            cr_tmp |= (CR0_BURST_LENGTH_16_CLK << CR0_BURST_LENGTH_SHIFT);
        } else if (cfg->wrapSize == MCR_WRAPSIZE_32_CLK) {
            cr_tmp |= (CR0_BURST_LENGTH_32_CLK << CR0_BURST_LENGTH_SHIFT);
        }

        hyperWriteMR(regionMap, HYPER_PSRAM_CR0, cr_tmp);

        DSMC_HOST_CfgLatency(latency, latency, host, cs);

        cr_tmp = hyperReadMR(regionMap, HYPER_PSRAM_CR1);
        cr_tmp = (cr_tmp & (~(CR1_CLOCK_TYPE_MASK << CR1_CLOCK_TYPE_SHIFT))) |
                 (CR1_CLOCK_TYPE_DIFF_CLK << CR1_CLOCK_TYPE_SHIFT);
        hyperWriteMR(regionMap, HYPER_PSRAM_CR1, cr_tmp);
    }
    /* config to memory space */
    WRITE_REG(pRegCs->MCR, mcr);

    return 0;
}

static int DSMC_HOST_PsramInit(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t latency;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];
    uint32_t mhz = host->maxFreq / 1000000;

    if (mhz <= 66) {
        latency = 3;
    } else if (mhz <= 100) {
        latency = 4;
    } else if (mhz <= 133) {
        latency = 5;
    } else if (mhz <= 166) {
        latency = 6;
    } else if (mhz <= 200) {
        latency = 7;
    } else {
        HAL_DBG_ERR("DSMC_HOST: PSRAM frequency do not support!\n");

        return -1;
    }

    cfg->rdLatency = cfg->wrLatency = latency;

    DSMC_HOST_PsramCfg(host, cs);

    return 0;
}

static int DSMC_HOST_PsramCtrllerInit(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    int ret = 0;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_DEVTYPE_MASK,
               MCR_DEVTYPE_HYPERRAM << DSMC_MCR0_DEVTYPE_SHIFT);

    MODIFY_REG(pRegCs->VDMC,
               DSMC_VDMC0_MID_MASK |
               DSMC_VDMC0_PROTOCOL_MASK,
               ((cfg->mid << DSMC_VDMC0_MID_SHIFT) & DSMC_VDMC0_MID_MASK) |
               ((cfg->protcl << DSMC_VDMC0_PROTOCOL_SHIFT) & DSMC_VDMC0_PROTOCOL_MASK));
    WRITE_REG(pRegCs->MTR, cfg->mtrTiming);

    ret = DSMC_HOST_PsramInit(host, cs);

    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_WRAPSIZE_MASK,
               (cfg->wrapSize << DSMC_MCR0_WRAPSIZE_SHIFT) & DSMC_MCR0_WRAPSIZE_MASK);

    return ret;
}

static void DSMC_HOST_PsramRemodifyTiming(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    uint32_t maxLength = 511, tcmd = 3;
    uint32_t tcsm, tmp;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    uint32_t mhz = host->maxFreq / 1000000;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    if (cfg->mid == APM_PSRAM) {
        /* for extended temp */
        if (regionMap->size <= 0x400000) {
            tcsm = DSMC_DEC_TCEM_2_5U;
        } else if (regionMap->size <= 0x1000000) {
            tcsm = DSMC_DEC_TCEM_3U;
        } else {
            tcsm = DSMC_DEC_TCEM_0_5U;
        }
    } else {
        tcsm = DSMC_DEV_TCSM_1U;
    }

    tmp = (tcsm * mhz + 999) / 1000;
    tmp = tmp - tcmd - 2 * cfg->wrLatency - 4;

    if (tmp > maxLength) {
        tmp = maxLength;
    }

    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_MAXEN_MASK |
               DSMC_MCR0_MAXLEN_MASK |
               DSMC_MCR0_IOWIDTH_MASK,
               (MCR_MAX_LENGTH_EN << DSMC_MCR0_MAXEN_SHIFT) |
               ((tmp << DSMC_MCR0_MAXLEN_SHIFT) & DSMC_MCR0_MAXLEN_MASK) |
               ((cfg->ioWidth << DSMC_MCR0_IOWIDTH_SHIFT) & DSMC_MCR0_IOWIDTH_MASK));

    if (cfg->ioWidth == MCR_IOWIDTH_X16) {
        tmp = cfg->col - 2;
    } else {
        tmp = cfg->col - 1;
    }
    MODIFY_REG(pRegCs->BDRTCR,
               DSMC_BDRTCR0_COL_BIT_NUM_MASK |
               DSMC_BDRTCR0_WR_BDR_XFER_EN_MASK |
               DSMC_BDRTCR0_RD_BDR_XFER_EN_MASK,
               (((tmp - 6) << DSMC_BDRTCR0_COL_BIT_NUM_SHIFT) &
                DSMC_BDRTCR0_COL_BIT_NUM_MASK) |
               ((cfg->wrBdrXferEn << DSMC_BDRTCR0_WR_BDR_XFER_EN_SHIFT) &
                DSMC_BDRTCR0_WR_BDR_XFER_EN_MASK) |
               ((cfg->rdBdrXferEn << DSMC_BDRTCR0_RD_BDR_XFER_EN_SHIFT) &
                DSMC_BDRTCR0_RD_BDR_XFER_EN_MASK));
}

static void DSMC_HOST_LbClear_S2H_Intrupt(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    int region, manual = 0;
    uint32_t mcrTmp, rgnAttrTmp;
    struct DSMC_MAP *map;
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    region = DSMC_HOST_FindAttrRgn(cfg, RGNX_ATTR_REG);
    if (region < 0) {
        manual = -1;
        region = 0;
    }

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    if (manual) {
        mcrTmp = READ_REG(pRegCs->MCR);
        rgnAttrTmp = READ_REG(pRegCs->RGN0_ATTR);
        /* config to slave CSR space */
        MODIFY_REG(pRegCs->MCR,
                   DSMC_MCR0_CRT_MASK,
                   MCR_CRT_MEM_SPACE << DSMC_MCR0_CRT_SHIFT);
        MODIFY_REG(pRegCs->RGN0_ATTR,
                   RGNX_ATTR_MASK << RGNX_ATTR_SHIFT,
                   RGNX_ATTR_REG << RGNX_ATTR_SHIFT);
    }

    map = &host->dsmcHostDev.ChipSelMap[cs].regionMap[region];

    /* clear all s2h interrupt */
    lbWriteReg(map, LBC_S2H_INT_STA, 0x1 << S2H_INT_FOR_DMA_NUM);

    if (manual) {
        /* config to normal memory space */
        WRITE_REG(pRegCs->MCR, mcrTmp);
        WRITE_REG(pRegCs->RGN0_ATTR, rgnAttrTmp);
    }
}

static void DSMC_HOST_XccelaPsramReset(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;
    struct DSMC_MAP *regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    MODIFY_REG(pRegCs->VDMC,
               DSMC_VDMC0_RESET_CMD_MODE_MASK,
               0x1 << DSMC_VDMC0_RESET_CMD_MODE_SHIFT);
    xccelaWriteMR(regionMap, XCCELA_PSRAM_MR(0), XCCELA_PSRAM_MR_SET(0x0));
    HAL_CPUDelayUs(200);
    MODIFY_REG(pRegCs->VDMC,
               DSMC_VDMC0_RESET_CMD_MODE_MASK,
               0x0 << DSMC_VDMC0_RESET_CMD_MODE_SHIFT);
}

static HAL_Status DSMC_HOST_MapInit(struct HAL_DSMC_HOST *host)
{
    uint32_t cs, rgn, rgnNumMax;
    uint32_t num = 0;
    struct DSMC_MAP *regionMap;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;

    rgnNumMax = 1;
    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++) {
        if (dsmcHostDev->ChipSelCfg[cs].deviceType != DEV_DSMC_LB) {
            continue;
        }
        for (rgn = 0; rgn < DSMC_LB_MAX_RGN; rgn++) {
            if (dsmcHostDev->ChipSelCfg[cs].slvRgn[rgn].status) {
                dsmcHostDev->ChipSelCfg[cs].rgnNum++;
            }
        }
        if (dsmcHostDev->ChipSelCfg[cs].rgnNum == 3) {
            dsmcHostDev->ChipSelCfg[cs].rgnNum++;
        }
        rgnNumMax = HAL_MAX(rgnNumMax, dsmcHostDev->ChipSelCfg[cs].rgnNum);
    }

    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++) {
        if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_XCCELA_PSRAM) {
            regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[0];
            regionMap->phys = host->regionMem.regionBase +
                              host->regionMem.regionSize * cs;
            regionMap->size = host->regionMem.regionSize;
            dsmcHostDev->cap = HAL_MAX(dsmcHostDev->cap, regionMap->size);
        } else if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_DSMC_LB) {
            num = 0;
            for (rgn = 0; rgn < DSMC_LB_MAX_RGN; rgn++) {
                if (!dsmcHostDev->ChipSelCfg[cs].slvRgn[rgn].status) {
                    continue;
                }
                regionMap = &dsmcHostDev->ChipSelMap[cs].regionMap[rgn];
                regionMap->phys = host->regionMem.regionBase +
                                  rgnNumMax * host->regionMem.regionSize * cs +
                                  num * host->regionMem.regionSize;
                regionMap->size = host->regionMem.regionSize;
                dsmcHostDev->cap = HAL_MAX(dsmcHostDev->cap, regionMap->size);
                num++;
            }
            if (!num) {
                HAL_DBG_ERR("DSMC_HOST: cs%" PRIu32 " all regions are disabled\n", cs);

                return HAL_ERROR;
            }
        }
    }
    dsmcHostDev->cap *= rgnNumMax;

    return HAL_OK;
}

static HAL_Status DSMC_HOST_DataInit(struct HAL_DSMC_HOST *host)
{
    uint32_t cs, unDevNum = 0;
    HAL_Status ret = HAL_OK;
    struct DSMC_CONFIG_CS *ChipSelCfg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev = &host->dsmcHostDev;

    ret = DSMC_HOST_MapInit(host);
    if (ret != HAL_OK) {
        return HAL_ERROR;
    }

    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++) {
        if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_UNKNOWN) {
            unDevNum++;
            continue;
        }

        ChipSelCfg = &dsmcHostDev->ChipSelCfg[cs];
        ChipSelCfg->exclusiveDQS = 0;
        ChipSelCfg->ioWidth = dsmcHostDev->ioWidth;

        if (ChipSelCfg->deviceType == DEV_HYPER_PSRAM) {
            ChipSelCfg->deviceType = DEV_XCCELA_PSRAM;
        }

        if (ChipSelCfg->deviceType == DEV_XCCELA_PSRAM) {
            ChipSelCfg->wrapSize = DSMC_BURST_WRAPSIZE_32CLK;
            ChipSelCfg->wrap2incrEn = 0;
            ChipSelCfg->acs = 1;
            ChipSelCfg->maxLengthEn = 1;
            ChipSelCfg->maxLength = 0xff;
            ChipSelCfg->rdBdrXferEn = 1;
            ChipSelCfg->wrBdrXferEn = 1;
        } else {
            ChipSelCfg->wrapSize = DSMC_BURST_WRAPSIZE_16CLK;
            ChipSelCfg->wrap2incrEn = 1;
            ChipSelCfg->acs = 1;
            ChipSelCfg->maxLengthEn = 0;
            ChipSelCfg->maxLength = 0x0;
        }
    }
    if (unDevNum == DSMC_MAX_SLAVE_NUM) {
        HAL_DBG_ERR("DSMC_HOST: All device types are unknown.\n");

        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

static void DSMC_HOST_CtrllerInit(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_CS_REG *pRegCs;
    struct DSMC_REG *pReg = host->pReg;
    struct DSMC_CONFIG_CS *cfg = &host->dsmcHostDev.ChipSelCfg[cs];

    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    WRITE_REG(pRegCs->MRGTCR, MRGTCR_READ_WRITE_MERGE_EN);
    WRITE_REG(pRegCs->RDS_DLL0_CTL,
              (0x1 << DSMC_RDS_DLL0_CTL0_RDS_0_CLK_SMP_SEL_SHIFT) |
              ((cfg->dqsDLL.rxDLLDQS[0] << DSMC_RDS_DLL0_CTL0_RDS_0_CLK_DELAY_NUM_SHIFT) &
               DSMC_RDS_DLL0_CTL0_RDS_0_CLK_DELAY_NUM_MASK));
    WRITE_REG(pRegCs->RDS_DLL1_CTL,
              (0x1 << DSMC_RDS_DLL1_CTL0_RDS_1_CLK_SMP_SEL_SHIFT) |
              ((cfg->dqsDLL.rxDLLDQS[1] << DSMC_RDS_DLL1_CTL0_RDS_1_CLK_DELAY_NUM_SHIFT) &
               DSMC_RDS_DLL1_CTL0_RDS_1_CLK_DELAY_NUM_MASK));

    MODIFY_REG(pRegCs->MCR,
               DSMC_MCR0_ACS_MASK |
               DSMC_MCR0_DEVTYPE_MASK |
               DSMC_MCR0_IOWIDTH_MASK |
               DSMC_MCR0_EXCLUSIVE_DQS_MASK |
               DSMC_MCR0_WRAPSIZE_MASK |
               DSMC_MCR0_MAXEN_MASK |
               DSMC_MCR0_MAXLEN_MASK,
               ((cfg->acs << DSMC_MCR0_ACS_SHIFT) & DSMC_MCR0_ACS_MASK) |
               (MCR_DEVTYPE_HYPERRAM << DSMC_MCR0_DEVTYPE_SHIFT) |
               ((cfg->ioWidth << DSMC_MCR0_IOWIDTH_SHIFT) & DSMC_MCR0_IOWIDTH_MASK) |
               ((cfg->exclusiveDQS << DSMC_MCR0_EXCLUSIVE_DQS_SHIFT) &
                DSMC_MCR0_EXCLUSIVE_DQS_MASK) |
               ((cfg->wrapSize << DSMC_MCR0_WRAPSIZE_SHIFT) & DSMC_MCR0_WRAPSIZE_MASK) |
               ((cfg->maxLengthEn << DSMC_MCR0_MAXEN_SHIFT) & DSMC_MCR0_MAXEN_MASK) |
               ((cfg->maxLength << DSMC_MCR0_MAXLEN_SHIFT) & DSMC_MCR0_MAXLEN_MASK));

    WRITE_REG(pRegCs->WRAP2INCR, cfg->wrap2incrEn);

    MODIFY_REG(pRegCs->VDMC,
               DSMC_VDMC0_LATENCY_FIXED_MASK |
               DSMC_VDMC0_PROTOCOL_MASK,
               (VDMC_LATENCY_VARIABLE << DSMC_VDMC0_LATENCY_FIXED_SHIFT) |
               ((cfg->deviceType << DSMC_VDMC0_PROTOCOL_SHIFT) &
                DSMC_VDMC0_PROTOCOL_MASK));

    WRITE_REG(pReg->DEV_SIZE, DSMC_HOST_cap2devSize(host->dsmcHostDev.cap));
}

static int DSMC_HOST_DectectDev(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    int ret = 0;

    DSMC_HOST_CtrllerInit(host, cs);
    ret = DSMC_HOST_PsramDectect(host, cs);
    if (ret) {
        return ret;
    }

    return ret;
}

static int DSMC_HOST_PsramReInit(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    int ret = 0;

    if (host->dsmcHostDev.ChipSelCfg[cs].protcl == OPI_XCCELA_PSRAM) {
        DSMC_HOST_XccelaPsramReset(host, cs);
    }
    ret = DSMC_HOST_PsramCtrllerInit(host, cs);
    DSMC_HOST_PsramRemodifyTiming(host, cs);

    return ret;
}

static void DSMC_HOST_ResetCtrl(struct HAL_DSMC_HOST *host)
{
#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkResetAssert(host->arst);
    HAL_CRU_ClkResetAssert(host->prst);
    HAL_CPUDelayUs(20);
    HAL_CRU_ClkResetDeassert(host->arst);
    HAL_CRU_ClkResetDeassert(host->prst);
#endif
}

static int DSMC_HOST_LbInit(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    int ret = 0;

    DSMC_HOST_LbCtrllerCfg(host, cs);
    ret = DSMC_HOST_LbCmnConfig(host, cs);
    if (ret) {
        return ret;
    }
    DSMC_HOST_LbCsrConfig(host, cs);

    return ret;
}

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup DSMC_HOST_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  DSMC_HOST init.
 * @param  host: pointer to a DSMC_HOST structure.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_Init(struct HAL_DSMC_HOST *host)
{
    int ret = 0;
    uint32_t cs;
    struct DSMC_CONFIG_CS *ChipSelCfg;
    struct HAL_DSMC_HOST_DEV *dsmcHostDev;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(host->clkGate);
    HAL_CRU_ClkEnable(host->aclkGate);
    HAL_CRU_ClkEnable(host->pClkGate);
#endif

    dsmcHostDev = &host->dsmcHostDev;

    if (DSMC_HOST_DataInit(host)) {
        return HAL_ERROR;
    }

    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++) {
        if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_UNKNOWN) {
            continue;
        }
        if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_XCCELA_PSRAM) {
            ret = DSMC_HOST_DectectDev(host, cs);
            if (ret) {
                return HAL_ERROR;
            }
        }
    }
    DSMC_HOST_ResetCtrl(host);

    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++) {
        if (dsmcHostDev->ChipSelCfg[cs].deviceType == DEV_UNKNOWN) {
            continue;
        }
        ChipSelCfg = &dsmcHostDev->ChipSelCfg[cs];
        HAL_SYSLOG("DSMC_HOST: init cs%" PRIu32 " %s device\n",
                   cs, (ChipSelCfg->deviceType == DEV_DSMC_LB) ? "LB" : "PSRAM");
        DSMC_HOST_CtrllerInit(host, cs);
        if (ChipSelCfg->deviceType == DEV_XCCELA_PSRAM) {
            ret = DSMC_HOST_PsramReInit(host, cs);
        } else {
            ret = DSMC_HOST_LbInit(host, cs);
        }

        if (ret) {
            break;
        }
    }

    if (ret) {
        return HAL_ERROR;
    } else {
        return HAL_OK;
    }
}

/**
 * @brief  DSMC_HOST DeInit.
 * @param  host: pointer to a DSMC_HOST structure.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_DeInit(struct HAL_DSMC_HOST *host)
{
    HAL_ASSERT(host != NULL);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    DSMC_HOST_ResetCtrl(host);

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkDisable(host->clkGate);
    HAL_CRU_ClkDisable(host->aclkGate);
    HAL_CRU_ClkDisable(host->pClkGate);
#endif

    return HAL_OK;
}

/** @} */

/** @defgroup DSMC_HOST_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  DSMC_HOST write to slave to trigger a s2h interrupt.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  cs: operation of chip select.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_LbDmaTrigger(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_REG *pReg;
    struct DSMC_CS_REG *pRegCs;
    int region, manual = 0;
    uint32_t mcrTmp, rgnAttrTmp, flagTmp;
    struct DSMC_MAP *map;
    struct DSMC_CONFIG_CS *cfg;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    pReg = host->pReg;
    cfg = &host->dsmcHostDev.ChipSelCfg[cs];
    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    region = DSMC_HOST_FindAttrRgn(cfg, RGNX_ATTR_REG);
    if (region < 0) {
        manual = -1;
        region = 0;
    }

    if (manual) {
        mcrTmp = READ_REG(pRegCs->MCR);
        rgnAttrTmp = READ_REG(pRegCs->RGN0_ATTR);
        /* config to slave CSR space */
        MODIFY_REG(pRegCs->MCR,
                   DSMC_MCR0_CRT_MASK,
                   MCR_CRT_MEM_SPACE << DSMC_MCR0_CRT_SHIFT);
        MODIFY_REG(pRegCs->RGN0_ATTR,
                   RGNX_ATTR_MASK << RGNX_ATTR_SHIFT,
                   RGNX_ATTR_REG << RGNX_ATTR_SHIFT);
    }
    map = &host->dsmcHostDev.ChipSelMap[cs].regionMap[region];
    /*
     * write (read(LBC_CON(15)) + 1) to LBC_CON15 to slave which will
     * write APP_CON(S2H_INT_FOR_DMA_NUM) trigger a slave to host interrupt
     */
    flagTmp = lbReadReg(map, LBC_CON(15));
    lbWriteReg(map, LBC_CON(15), flagTmp + 1);

    if (manual) {
        /* config to normal memory space */
        WRITE_REG(pRegCs->MCR, mcrTmp);
        WRITE_REG(pRegCs->RGN0_ATTR, rgnAttrTmp);
    }

    return 0;
}

/**
 * @brief  DSMC_HOST enable DMA request.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  size: DMA size.
 * @param  opsDma: DMA request number.
 * @param  cs: operation of chip select.
 * @return HAL_Status
 */
HAL_Status HAL_DSMC_HOST_EnableDmaRequest(struct HAL_DSMC_HOST *host, uint32_t size,
                                          uint32_t opsDma, uint32_t cs)
{
    uint32_t dma_req_num;
    struct DSMC_REG *pReg;
    volatile uint32_t *regAddr;
    uint32_t burst_byte;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(opsDma < DSMC_MAX_DMA_REQ_NUM);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    pReg = host->pReg;
    burst_byte = host->dmaReq[opsDma].brstLen * host->dmaReq[opsDma].brstSize;

    dma_req_num = size / burst_byte;
    if (size % burst_byte) {
        HAL_SYSLOG("DSMC_HOST: DMA size is unaligned\n");
        dma_req_num++;
    }
    regAddr = &pReg->DMA_REQ_NUM0 + opsDma;
    WRITE_REG(*regAddr, dma_req_num);

    /* enable dma request */
    MODIFY_REG(pReg->DMA_EN, DMA_REQ_MASK(cs), DMA_REQ_EN(cs));

    return HAL_OK;
}

/**
 * @brief  DSMC_HOST disable DMA request.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  cs: operation of chip select.
 * @return HAL_Status
 */
HAL_Status HAL_DSMC_HOST_DisableDmaRequest(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_REG *pReg;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    pReg = host->pReg;

    /* clear dsmc interrupt */
    MODIFY_REG(pReg->INT_STATUS, INT_STATUS_MASK(cs), INT_STATUS(cs));
    /* disable dma request */
    MODIFY_REG(pReg->DMA_EN, DMA_REQ_MASK(cs), DMA_REQ_DIS(cs));

    DSMC_HOST_LbClear_S2H_Intrupt(host, cs);

    return HAL_OK;
}

/**
 * @brief  DSMC_HOST mask interrupt.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  cs: operation of chip select.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_InterruptMask(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_REG *pReg;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    pReg = host->pReg;
    /* mask dsmc interrupt */
    MODIFY_REG(pReg->INT_MASK, INT_MASK(cs), INT_MASK(cs));

    return HAL_OK;
}

/**
 * @brief  DSMC_HOST unmask interrupt.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  cs: operation of chip select.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_InterruptUnmask(struct HAL_DSMC_HOST *host, uint32_t cs)
{
    struct DSMC_REG *pReg;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));

    pReg = host->pReg;
    /* unmask dsmc interrupt */
    MODIFY_REG(pReg->INT_MASK, INT_MASK(cs), INT_UNMASK(cs));

    return HAL_OK;
}

/**
 * @brief  DSMC_HOST setting the DLL of byte.
 * @param  host: pointer to a DSMC_HOST structure.
 * @param  cs: operation of chip select.
 * @param  byte: target byte.
 * @param  dll: DLL value.
 * @return HAL_Status.
 */
HAL_Status HAL_DSMC_HOST_DllSetting(struct HAL_DSMC_HOST *host, uint32_t cs,
                                    uint32_t byte, uint32_t dll)
{
    volatile uint32_t *regAddr;
    struct DSMC_REG *pReg;
    struct DSMC_CS_REG *pRegCs;

    HAL_ASSERT(host != NULL);
    HAL_ASSERT(cs < DSMC_MAX_SLAVE_NUM);
    HAL_ASSERT(byte < DSMC_MAX_BYTE);
    HAL_ASSERT(IS_DSMC_INSTANCE(host->pReg));
    pReg = host->pReg;
    pRegCs = (struct DSMC_CS_REG *)DSMC_HOST_GetCsReg(pReg, cs);

    regAddr = &pRegCs->RDS_DLL0_CTL + byte;
    MODIFY_REG(*regAddr,
               DSMC_RDS_DLL0_CTL0_RDS_0_CLK_DELAY_NUM_MASK,
               (dll << DSMC_RDS_DLL0_CTL0_RDS_0_CLK_DELAY_NUM_SHIFT) &
               DSMC_RDS_DLL0_CTL0_RDS_0_CLK_DELAY_NUM_MASK);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_DSMC_HOST_MODULE_ENABLED */
