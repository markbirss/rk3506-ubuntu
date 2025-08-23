/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _RK960_FIRMWARE_H_
#define _RK960_FIRMWARE_H_

//#define ENABLE_LOADER_BEFORE_FIRMWARE
//#define FW_LOADER_FROM_FOPEN

//#define FW_MEMORY_CHECK
#define FW_DOWNLOAD_CHECK

#define MAX_FW_BUF_SIZE_SMALL (16*1024)
#define MAX_FW_BUF_SIZE_BIG   (126*1024)
#define MAX_FW_DATA_SIZE      (256*1024)
#define MAX_FW_RFCAL_DATA_SIZE  (64*1024)
#define MAX_LOADER_DATA_SIZE  MAX_FW_BUF_SIZE_SMALL
#define MAX_SDD_BUF_SIZE      (4*1024)

#define SDIO_HOST_READ_REG_FW_STATE			64

#define SDIO_LOADER_VCT_ADDR		(86*1024)
#define SDIO_FIRMWARE_VCT_ADDR		0x0000

#define SDIO_CMD_ADDR_VER_ABC		0x10000
#define SDIO_CMD_ADDR_VER_D			(255*1024/512)
#define SDIO_CMD_ADDR_VER_ROM		(95*1024/512)
#define SDIO_ROM_VER_ADDR           (0x40000/512)

#define SDIO_HOST_PATCH_ADDR	0
#define SDIO_START_CMD_ID		0x5A5A5A5A
#define SDIO_HOST_TO_LOADER_CMD	0x5A5A5A5D

/* hwio addr info */
#define IO_RECV_LEN_L 			2
#define IO_RECV_LEN_H 			3

#define IO_INT_ADDR			32
#define IO_INT_CLR_IRQ_VAL		2

#define IO_NOTIFY_ADDR      17
#define IO_NOTIFY_VAL       1
#define IO_NOTIFY_SLEEP     2
#define IO_NOTIFY_WAKEUP    3
#define IO_NOTIFY_JTAG      4
#define IO_NOTIFY_RESET     5
#define IO_NOTIFY_PWRDOWN   6

#define IO_WRITE_REQ_INT_STA		35

struct firmware_info {
	int fw_saved;
	unsigned char *fw_data;
	int fw_size;
	unsigned char *fw_rfcal_data;
	int fw_rfcal_size;
	unsigned char *buf_data;
	int buf_size;
	int useful_code_size;
	unsigned char *loder_data;
	int loder_size;
	unsigned char *sdd_data;
	int sdd_size;
#ifdef FW_DOWNLOAD_CHECK
	unsigned char *fw_data_check;
#endif
	unsigned char *fw_start_data;
	const struct firmware *fw_data_r;
	const struct firmware *fw_rfcal_data_r;
        const struct firmware *loder_data_r;
        const struct firmware *sdd_data_r;
};

/* SDD definitions */
#define SDD_PTA_CFG_ELT_ID 0xEB
#define SDD_REFERENCE_FREQUENCY_ELT_ID 0xc5

int rk960_load_firmware(struct rk960_common *priv);
int rk960_start_fw(struct rk960_common *priv);
int rk960_download_fw(struct rk960_common *priv, int start_fw);
int rk960_alloc_firmware_buf(struct firmware_info *fw_info);
void rk960_free_firmware_buf(struct firmware_info *fw_info);

#endif
