/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_DSMC_HOST_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DSMC_HOST
 *  @{
 */

#ifndef _HAL_DSMC_HOST_H_
#define _HAL_DSMC_HOST_H_

#include "hal_base.h"

/***************************** MACRO Definition ******************************/
/** @defgroup DSMC_HOST_Exported_Definition_Group1 Basic Definition
 *  @{
 */

#define DSMC_FPGA_WINBOND_X8  0
#define DSMC_FPGA_WINBOND_X16 1
#define DSMC_FPGA_APM_X8      0
#define DSMC_FPGA_APM_X16     0

#define DSMC_FPGA_DMAC_TEST 1

#define DSMC_MAX_SLAVE_NUM   4
#define DSMC_LB_MAX_RGN      4
#define DSMC_MAX_BYTE        2
#define DSMC_MAX_DMA_REQ_NUM 2

#define DSMC_MEM_ATTRIBUTE_NO_CACHE 0
#define DSMC_MEM_ATTRIBUTE_CACHE    1
#define DSMC_MEM_ATTRIBUTE_WR_COM   2

#define S2H_INT_FOR_DMA_NUM (15)

#define DSMC_DMA_MUX 0x005c

#define DSMC_RDS_DLL_CTL(cs, byte) (0x1000 * ((cs) + 1) + 0x30 + (byte) * 0x4)

#define DSMC_UNKNOWN_DEVICE 0x0
#define OPI_XCCELA_PSRAM    0x1
#define HYPERBUS_PSRAM      0x2
#define DSMC_LB_DEVICE      0x3

#define MCR_CRT_CR_SPACE     0x1
#define MCR_CRT_MEM_SPACE    0x0
#define MCR_IOWIDTH_X16      0x1
#define MCR_IOWIDTH_X8       0x0
#define MCR_DEVTYPE_HYPERRAM 0x1
#define MCR_MAX_LENGTH_EN    0x1
#define MCR_MAX_LENGTH       0x1ff

#define DSMC_BURST_WRAPSIZE_32CLK 0x1
#define DSMC_BURST_WRAPSIZE_8CLK  0x2
#define DSMC_BURST_WRAPSIZE_16CLK 0x3

/* RGNX_ATTR */
#define RGNX_ATTR_SHIFT             0
#define RGNX_ATTR_MASK              0x3
#define RGNX_ATTR_REG               0x0
#define RGNX_ATTR_DPRA              0x1
#define RGNX_ATTR_NO_MERGE_FIFO     0x2
#define RGNX_ATTR_MERGE_FIFO        0x3
#define RGNX_ATTR_CTRL_SHIFT        4
#define RGNX_ATTR_BE_CTRLED_SHIFT   5
#define RGNX_ATTR_DUM_CLK_EN_SHIFT  6
#define RGNX_ATTR_DUM_CLK_NUM_SHIFT 7
#define RGNX_ATTR_CA_ADDR_MASK      1
#define RGNX_ATTR_32BIT_ADDR_WIDTH  0
#define RGNX_ATTR_16BIT_ADDR_WIDTH  1
#define RGNX_ATTR_ADDR_WIDTH_SHIFT  8

#define RGNX_STATUS_ENABLED  (1)
#define RGNX_STATUS_DISABLED (0)

/***************************** Structure Definition **************************/

typedef enum {
    DEV_UNKNOWN = 0,
    DEV_XCCELA_PSRAM,
    DEV_HYPER_PSRAM,
    DEV_DSMC_LB,
} eDSMC_devType;

struct REGIONS_CONFIG {
    uint32_t attribute;
    uint32_t caAddrWidth;
    uint32_t dummyClkNum;
    uint32_t cs0BeCtrled;
    uint32_t cs0Ctrl;
    uint32_t status;
};

struct DSMC_DQS_DLL {
    uint32_t rxDLLDQS[DSMC_MAX_BYTE];
};

struct DSMC_CONFIG_CS {
    uint16_t mid;
    uint16_t protcl;
    eDSMC_devType deviceType;
    uint32_t mtrTiming;
    uint32_t acs;
    uint32_t exclusiveDQS;
    uint32_t ioWidth;
    uint32_t wrapSize;
    uint32_t rcshi;
    uint32_t wcshi;
    uint32_t rcss;
    uint32_t wcss;
    uint32_t rcsh;
    uint32_t wcsh;
    uint32_t rdLatency;
    uint32_t wrLatency;
    uint32_t col;
    uint32_t wrap2incrEn;
    uint32_t maxLengthEn;
    uint32_t maxLength;
    uint32_t rgnNum;
    uint32_t rdBdrXferEn;
    uint32_t wrBdrXferEn;
    uint32_t intEn;
    struct DSMC_DQS_DLL dqsDLL;
    struct REGIONS_CONFIG slvRgn[DSMC_LB_MAX_RGN];
};

struct DSMC_MAP {
    void *virt;
    uintptr_t phys;
    size_t size;
};

struct DSMC_CS_MAP {
    struct DSMC_MAP regionMap[DSMC_LB_MAX_RGN];
};

struct DSMC_MEM_REGION {
    uintptr_t regionBase;
    size_t regionSize;
};

struct HAL_DSMC_HOST_DEV {
    eDSMC_devType type;
    uint32_t clkMode;
    uint32_t ctrlFreqHz;
    uint32_t cap;
    uint32_t dmaReqMuxOffset;
    uint32_t ioWidth;
    uint32_t wrapSize;
    struct DSMC_CONFIG_CS ChipSelCfg[DSMC_MAX_SLAVE_NUM];
    struct DSMC_CS_MAP ChipSelMap[DSMC_MAX_SLAVE_NUM];
};

struct HAL_DSMC_DMA_DATA {
    uint32_t status;        /**< Specifies DMA status */
    uint16_t brstLen;       /**< Specifies DMA width of the addr */
    uint8_t brstSize;       /**< Specifies DMA max number of words */
    uint8_t dmaReqCh;       /**< Specifies DMA request channel */
    uint32_t srcAddr;       /**< Specifies DMA src address */
    uint32_t dstAddr;       /**< Specifies DMA dst address */
    uint32_t transferSize;  /**< Specifies DMA transfer size */
    struct DMA_REG *dmac; /**< Specifies DMAC reg base address */
};

/** @brief  DSMC_HOST handle Structure definition */
struct HAL_DSMC_HOST {
    struct DSMC_REG *pReg;                      /**< Specifies DSMC_HOST register base address */
    uint32_t clkGate;                           /**< Specifies DSMC_HOST clk Gate */
    uint32_t aclkGate;                          /**< Specifies DSMC_HOST aclk Gate */
    uint32_t pClkGate;                          /**< Specifies DSMC_HOST pclk Gate */
    uint32_t maxFreq;                           /**< Specifies DSMC_HOST clock frequency. */
    eCLOCK_Name sclkID;                         /**< Specifies DSMC_HOST sclk ID */
    eCLOCK_Name sclkRootID;                     /**< Specifies DSMC_HOST root sclk ID */
    uint32_t arst;                              /**< Specifies DSMC_HOST areset */
    uint32_t prst;                              /**< Specifies DSMC_HOST preset */
    IRQn_Type irqNum;                           /**< Specifies DSMC_HOST irq number */
    struct DSMC_MEM_REGION regionMem;           /**< Specifies DSMC_HOST memory region */
    struct HAL_DSMC_DMA_DATA dmaReq[DSMC_MAX_DMA_REQ_NUM];  /**< Specifies DSMC_HOST DMA config */
    struct HAL_DSMC_HOST_DEV dsmcHostDev;       /**< Specifies DSMC_HOST device config */
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup DSMC_HOST_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_DSMC_HOST_Init(struct HAL_DSMC_HOST *host);
HAL_Status HAL_DSMC_HOST_DeInit(struct HAL_DSMC_HOST *host);
HAL_Status HAL_DSMC_HOST_EnableDmaRequest(struct HAL_DSMC_HOST *host, uint32_t size,
                                          uint32_t opsDma, uint32_t cs);
HAL_Status HAL_DSMC_HOST_DisableDmaRequest(struct HAL_DSMC_HOST *host, uint32_t cs);
HAL_Status HAL_DSMC_HOST_DllSetting(struct HAL_DSMC_HOST *host, uint32_t cs,
                                    uint32_t byte, uint32_t dll);
HAL_Status HAL_DSMC_HOST_InterruptMask(struct HAL_DSMC_HOST *host, uint32_t cs);
HAL_Status HAL_DSMC_HOST_InterruptUnmask(struct HAL_DSMC_HOST *host, uint32_t cs);
HAL_Status HAL_DSMC_HOST_LbDmaTrigger(struct HAL_DSMC_HOST *host, uint32_t cs);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_DSMC_HOST_MODULE_ENABLED */
