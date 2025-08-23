/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_SNOR_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SNOR
 *  @{
 */

#ifndef _HAL_SFC_NOR_H_
#define _HAL_SFC_NOR_H_

#include "hal_def.h"
#include "hal_spi_mem.h"

/***************************** MACRO Definition ******************************/
/** @defgroup SNOR_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

#define SPI_NOR_MAX_CMD_SIZE    8
#define SNOR_SPEED_MAX          133000000
#define SNOR_SPEED_DEFAULT      80000000
#define SNOR_ID_LENGTH_DEFAULT  4
#define SNOR_ID_LENGTH_MAX      6
#define SNOR_ID_LENGTH_MACRONIX SNOR_ID_LENGTH_MAX

#define SNOR_PROTO_IS_DTR HAL_BIT(24)   /* Double Transfer Rate */

#define SNOR_PROTO_STR(a, b, c) (((a) << 8) | ((b) << 4) | (c))
#define SNOR_PROTO_DTR(a, b, c) \
    (SNOR_PROTO_IS_DTR |        \
     SNOR_PROTO_STR(a, b, c))

#define SNOR_GET_PROTOCOL_CMD_BITS(proto)  ((proto) >> 8 & 0xf)
#define SNOR_GET_PROTOCOL_ADDR_BITS(proto) (((proto) >> 4) & 0xf)
#define SNOR_GET_PROTOCOL_DATA_BITS(proto) ((proto) & 0xf)

enum SPI_NOR_PROTOCOL {
    SNOR_PROTO_1_1_1 = SNOR_PROTO_STR(1, 1, 1),
    SNOR_PROTO_1_1_2 = SNOR_PROTO_STR(1, 1, 2),
    SNOR_PROTO_1_1_4 = SNOR_PROTO_STR(1, 1, 4),
    SNOR_PROTO_1_1_8 = SNOR_PROTO_STR(1, 1, 8),
    SNOR_PROTO_1_2_2 = SNOR_PROTO_STR(1, 2, 2),
    SNOR_PROTO_1_4_4 = SNOR_PROTO_STR(1, 4, 4),
    SNOR_PROTO_1_8_8 = SNOR_PROTO_STR(1, 8, 8),
    SNOR_PROTO_2_2_2 = SNOR_PROTO_STR(2, 2, 2),
    SNOR_PROTO_4_4_4 = SNOR_PROTO_STR(4, 4, 4),
    SNOR_PROTO_8_8_8 = SNOR_PROTO_STR(8, 8, 8),

    SNOR_PROTO_1_1_1_DTR = SNOR_PROTO_DTR(1, 1, 1),
    SNOR_PROTO_1_2_2_DTR = SNOR_PROTO_DTR(1, 2, 2),
    SNOR_PROTO_1_4_4_DTR = SNOR_PROTO_DTR(1, 4, 4),
    SNOR_PROTO_1_8_8_DTR = SNOR_PROTO_DTR(1, 8, 8),
    SNOR_PROTO_8_8_8_DTR = SNOR_PROTO_DTR(8, 8, 8),
};

static inline bool HAL_SNOR_ProtocolIsDtr(enum SPI_NOR_PROTOCOL proto)
{
    return !!(proto & SNOR_PROTO_IS_DTR);
}

/* Flash opcodes. */
#define SPINOR_OP_WREN       0x06 /**< Write enable */
#define SPINOR_OP_RDSR       0x05 /**< Read status register */
#define SPINOR_OP_WRSR       0x01 /**< Write status register 1 byte */
#define SPINOR_OP_RDSR1      0x35 /**< Read status register 1 */
#define SPINOR_OP_WRSR1      0x31 /**< Write status register 1 */
#define SPINOR_OP_RDSR2      0x15 /**< Read status register 2 */
#define SPINOR_OP_WRSR2      0x3e /**< Write status register 2 */
#define SPINOR_OP_READ       0x03 /**< Read data bytes (low frequency) */
#define SPINOR_OP_READ_FAST  0x0b /**< Read data bytes (high frequency) */
#define SPINOR_OP_READ_1_1_2 0x3b /**< Read data bytes (Dual Output SPI) */
#define SPINOR_OP_DUALRD4B   0x3c /**< Read data bytes (Dual Output SPI) with byte address */
#define SPINOR_OP_READ_1_2_2 0xbb /**< Read data bytes (Dual I/O SPI) */
#define SPINOR_OP_READ_1_1_4 0x6b /**< Read data bytes (Quad Output SPI) */
#define SPINOR_OP_READ_1_4_4 0xeb /**< Read data bytes (Quad I/O SPI) */
#define SPINOR_OP_READ_EC    0xec /**< Read data bytes (Quad I/O SPI) 4 byte address */
#define SPINOR_OP_4DTRD4B    0xee /**< Read data bytes (Quad I/O DDR SPI) 4 byte address */
#define SPINOR_OP_PP         0x02 /**< Page program (up to 256 bytes) */
#define SPINOR_OP_PP_1_1_4   0x32 /**< Quad page program */
#define SPINOR_OP_PP_1_4_4   0x38 /**< Quad page program */
#define SPINOR_OP_4PP_1_4_4  0x3e /**< Quad page program with 4B */
#define SPINOR_OP_BE_4K      0x20 /**< Erase 4KiB block */
#define SPINOR_OP_BE_4K_PMC  0xd7 /**< Erase 4KiB block on PMC chips */
#define SPINOR_OP_BE_32K     0x52 /**< Erase 32KiB block */
#define SPINOR_OP_CHIP_ERASE 0xc7 /**< Erase whole flash chip */
#define SPINOR_OP_SE         0xd8 /**< Sector erase (usually 64KiB) */
#define SPINOR_OP_RDID       0x9f /**< Read JEDEC ID */
#define SPINOR_OP_RDSFDP     0x5a /**< Read SFDP */
#define SPINOR_OP_RDCR       0x15 /**< Read configuration register */
#define SPINOR_OP_WRCR       0x11 /**< Write configuration register */
#define SPINOR_OP_RDFSR      0x70 /**< Read flag status register */
#define SPINOR_OP_CLFSR      0x50 /**< Clear flag status register */
#define SPINOR_OP_RDEAR      0xc8 /**< Read Extended Address Register */
#define SPINOR_OP_WREAR      0xc5 /**< Write Extended Address Register */
#define SPINOR_OP_READ_UUID  0x4b /**< Read SPI Nor UUID */
#define SPINOR_OP_READ_SFDP  0x5a /**< Read SPI Nor SFDP */
#define SPINOR_OP_EN_RESET   0x66 /**< Enable reset */
#define SPINOR_OP_RESET      0x99 /**< Reset devices */
#define SPINOR_OP_ENQPI35    0x35 /**< Enter qpi mode by 35h cmd */
#define SPINOR_OP_EXITQPIF5  0xf5 /**< Exit qpi mode by F5h cmd */
#define SPINOR_OP_ENQPI38    0x38 /**< Enter qpi mode by 38h cmd */
#define SPINOR_OP_EXITQPIFF  0xff /**< Exit qpi mode by FFh cmd */

#ifndef HAL_SNOR_SUPPORT_DEVICES_SELECT
#define HAL_SNOR_SUPPORT_GIGADEV
#define HAL_SNOR_SUPPORT_WINBOND
#define HAL_SNOR_SUPPORT_MXIC
#define HAL_SNOR_SUPPORT_XMC
#define HAL_SNOR_SUPPORT_XTX
#define HAL_SNOR_SUPPORT_EON
#define HAL_SNOR_SUPPORT_PUYA
#define HAL_SNOR_SUPPORT_ZBIT
#define HAL_SNOR_SUPPORT_HUAHONG
#define HAL_SNOR_SUPPORT_FUDAN
#define HAL_SNOR_SUPPORT_ISSI
#else
#if !defined(HAL_SNOR_SUPPORT_GIGADEV) && \
    !defined(HAL_SNOR_SUPPORT_GIGADEV) && \
    !defined(HAL_SNOR_SUPPORT_WINBOND) && \
    !defined(HAL_SNOR_SUPPORT_MXIC) &&    \
    !defined(HAL_SNOR_SUPPORT_XMC) &&     \
    !defined(HAL_SNOR_SUPPORT_XTX) &&     \
    !defined(HAL_SNOR_SUPPORT_EON) &&     \
    !defined(HAL_SNOR_SUPPORT_PUYA) &&    \
    !defined(HAL_SNOR_SUPPORT_ZBIT) &&    \
    !defined(HAL_SNOR_SUPPORT_HUAHONG) && \
    !defined(HAL_SNOR_SUPPORT_FUDAN) &&   \
    !defined(HAL_SNOR_SUPPORT_ISSI)
#error "select the specific spinor devices!"
#endif
#endif

struct OPI_DTR_OP_CODE {
    uint16_t iomodeAddr;
    uint16_t dummyAddr;
    uint8_t srAddrSize;
    uint8_t iomodeVal;
    uint8_t dummyVal;
    uint8_t dummySlow;
    uint8_t dummyFast;
    uint16_t readPage;
    uint16_t progPage;
    uint16_t eraseSec;
    uint16_t eraseBlk;
    uint16_t rdcr;
    uint16_t wrcr;
    uint16_t rdid;
    uint16_t wren;
    uint16_t rdsr;
};

struct SPI_NOR {
    struct SNOR_HOST *spi;
    const struct FLASH_INFO *info;
    uint32_t pageSize;
    uint8_t addrWidth;
    uint16_t eraseOpcodeSec;
    uint16_t eraseOpcodeBlk;
    uint8_t readDummy;
    uint16_t rdcrOpcode;
    uint16_t rden;
    uint16_t wrcrOpcode;
    uint16_t readOpcode;
    uint16_t programOpcode;
    uint16_t wrenOpcode;
    uint16_t rdsrOpcode;

    HAL_Status (*readReg)(struct SPI_NOR *nor, uint16_t opcode, uint8_t *buf, uint32_t len);
    HAL_Status (*readRegPoll)(struct SPI_NOR *nor, uint16_t opcode, uint8_t *buf, uint32_t len);
    HAL_Status (*writeReg)(struct SPI_NOR *nor, uint16_t opcode, uint8_t *buf, uint32_t len);

    int32_t (*read)(struct SPI_NOR *nor, uint32_t from,
                    uint32_t len, uint8_t *read_buf);
    int32_t (*write)(struct SPI_NOR *nor, uint32_t to,
                     uint32_t len, const uint8_t *write_buf);
    HAL_Status (*erase)(struct SPI_NOR *nor, uint32_t offs);

    enum SPI_NOR_PROTOCOL readProto;
    enum SPI_NOR_PROTOCOL writeProto;
    uint8_t cmdBuf[SPI_NOR_MAX_CMD_SIZE];
    bool dtr; /* Whether enable DTR */
    bool poll; /* Whether enable poll status feature */
    bool qpi; /* Whether enable QPI */
    bool swap; /* Whether enable data swap */

    const char *name;
    uint32_t size;
    uint32_t sectorSize;
    uint32_t eraseSize;

    const struct OPI_DTR_OP_CODE *opcode;
};

typedef enum {
    ERASE_SECTOR = 0,
    ERASE_BLOCK64K,
    ERASE_CHIP
} NOR_ERASE_TYPE;

struct SNOR_HOST {
    uint32_t speed;
    uint32_t mode;
    uint8_t flags;
    uint32_t xipMem; /**< XIP data mapped memory */
    uint32_t xipMemCode; /**< XIP code mapped memory */
    HAL_Status (*xfer)(struct SNOR_HOST *spi, struct HAL_SPI_MEM_OP *op);
    HAL_Status (*xipConfig)(struct SNOR_HOST *spi, struct HAL_SPI_MEM_OP *op, uint32_t on);

    void *userdata;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup SNOR_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_SNOR_Init(struct SPI_NOR *nor);
HAL_Status HAL_SNOR_DeInit(struct SPI_NOR *nor);
uint32_t HAL_SNOR_GetCapacity(struct SPI_NOR *nor);
HAL_Status HAL_SNOR_ReadID(struct SPI_NOR *nor, uint8_t *data);
int32_t HAL_SNOR_ReadData(struct SPI_NOR *nor, uint32_t from, void *buf, uint32_t len);
int32_t HAL_SNOR_ProgData(struct SPI_NOR *nor, uint32_t to, void *buf, uint32_t len);
int32_t HAL_SNOR_Read(struct SPI_NOR *nor, uint32_t sec, uint32_t nSec, void *pData);
int32_t HAL_SNOR_Write(struct SPI_NOR *nor, uint32_t sec, uint32_t nSec, void *pData);
int32_t HAL_SNOR_OverWrite(struct SPI_NOR *nor, uint32_t sec, uint32_t nSec, void *pData);
HAL_Status HAL_SNOR_Erase(struct SPI_NOR *nor, uint32_t addr, NOR_ERASE_TYPE EraseType);
HAL_Status HAL_SNOR_XIPEnable(struct SPI_NOR *nor);
HAL_Status HAL_SNOR_XIPDisable(struct SPI_NOR *nor);
HAL_Check HAL_SNOR_IsFlashSupported(uint8_t *flashId);
HAL_Status HAL_SNOR_ReadUUID(struct SPI_NOR *nor, void *buf);
HAL_Status HAL_SNOR_ResetDevice(struct SPI_NOR *nor);
bool HAL_SNOR_IsDtr(struct SPI_NOR *nor);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_SNOR_MODULE_ENABLED */
