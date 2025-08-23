/**
  * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: BSD-3-Clause
  ******************************************************************************
  * @file    spi2apb_flash_obj.h
  * @author  Jon Lin
  * @version V1.0
  * @date    2024-05-24
  * @brief   spi2apb driver for pisces
  *
  ******************************************************************************
  */

#ifndef __DRV_SPI2APB_FLASH_OBJ_H__
#define __DRV_SPI2APB_FLASH_OBJ_H__

#include <stdlib.h>
#include <rtthread.h>

#define SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE 4096 /* spinor 4KB sector */

#define SPI2APB_REG1_TRIGGER_INDEX 1 /* trigger reg1 */

/*
 * REG0(write msg reg0)
 */
struct spi2apb_flash_obj_crc
{
    rt_uint32_t crc;
};

/*
 * REG1(write msg reg1, trigger interrupt)
 */
typedef union
{
    rt_uint32_t d32;
    struct
    {
        rt_uint8_t index;
        rt_uint8_t cmd;
#define SPI2APB_FLASH_OBJ_CMD_READ 0x6B
#define SPI2APB_FLASH_OBJ_CMD_PROG 0x32
#define SPI2APB_FLASH_OBJ_CMD_RESET 0x96
#define SPI2APB_FLASH_OBJ_CMD_PROG_SRAM 0x12
        rt_uint16_t addr; /* flash address in 4KB sector */
    } item;
} spi2apb_flash_obj_packet;

enum spi2apb_flash_obj_master_req
{
    SPI2APB_FLASH_OBJ_MASTER_DATA_REQ = 0x10,
    SPI2APB_FLASH_OBJ_MASTER_RECOVERY = 0x20,
    SPI2APB_FLASH_OBJ_MASTER_EXIT,
};

#define SPI2APB_FLASH_OBJ_CMD_PROG_SRAM_PACKET (SPI2APB_FLASH_OBJ_CMD_PROG_SRAM << 8)

/*
 * REG2
 */
typedef union
{
    rt_uint32_t d32;
    struct
    {
        rt_uint8_t index;
        rt_uint8_t status;
#define SPI2APB_FLASH_OBJ_SLAVE_UNKNOWN 0
#define SPI2APB_FLASH_OBJ_SLAVE_IDLE 1
#define SPI2APB_FLASH_OBJ_SLAVE_BUSY 2
#define SPI2APB_FLASH_OBJ_SLAVE_READ_DATA_READY 3
#define SPI2APB_FLASH_OBJ_SLAVE_ERROR 4
#define SPI2APB_FLASH_OBJ_SLAVE_SUCCESS 5
#define SPI2APB_FLASH_OBJ_SLAVE_RECOVERY 6
#define SPI2APB_FLASH_OBJ_SLAVE_EXIT 7
        rt_uint16_t reserved;
    } item;
} spi2apb_flash_obj_status;

#define SPI2APB_FLASH_OBJ_CMD_READ_SRAM_READY (0 | SPI2APB_FLASH_OBJ_SLAVE_SUCCESS << 8) /* index 0 success, value 0h00000500 */

#endif