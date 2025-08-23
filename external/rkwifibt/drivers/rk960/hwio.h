/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_HWIO_H_INCLUDED
#define RK960_HWIO_H_INCLUDED

#include <linux/module.h>
#include <net/mac80211.h>
#include <linux/time.h>
#include <linux/pm.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/rfkill-wlan.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/suspend.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/delay.h>

#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/core.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sd.h>

/* ***************************************************************
*Device register definitions
*************************************************************** */
/* 16/32 bits */
#define ST90TDS_CONFIG_REG_ID		(0x0000)
/* 16/32 bits */
#define ST90TDS_CONTROL_REG_ID		(0x0001)

/* 32 bits, t_settle/general */
#define ST90TDS_TSET_GEN_R_W_REG_ID	(0x0006)

/* WBF - Control register bit set */
/* next o/p length, bit 11 to 0 */
#define ST90TDS_CONT_NEXT_LEN_MASK	(0xFFFF)
#define ST90TDS_CONT_WUP_BIT		(BIT(12))
#define ST90TDS_CONT_RDY_BIT		(BIT(13))
#define ST90TDS_CONT_IRQ_ENABLE		(BIT(14))
#define ST90TDS_CONT_RDY_ENABLE		(BIT(15))
#define ST90TDS_CONT_IRQ_RDY_ENABLE	(BIT(14)|BIT(15))

/* SPI Config register bit set */
#define ST90TDS_CONFIG_FRAME_BIT	(BIT(2))
#define ST90TDS_CONFIG_WORD_MODE_BITS	(BIT(3)|BIT(4))
#define ST90TDS_CONFIG_WORD_MODE_1	(BIT(3))
#define ST90TDS_CONFIG_WORD_MODE_2	(BIT(4))
#define ST90TDS_CONFIG_ERROR_0_BIT	(BIT(5))
#define ST90TDS_CONFIG_ERROR_1_BIT	(BIT(6))
#define ST90TDS_CONFIG_ERROR_2_BIT	(BIT(7))
/* TBD: Sure??? */
#define ST90TDS_CONFIG_CSN_FRAME_BIT	(BIT(7))
#define ST90TDS_CONFIG_ERROR_3_BIT	(BIT(8))
#define ST90TDS_CONFIG_ERROR_4_BIT	(BIT(9))
/* QueueM */
#define ST90TDS_CONFIG_ACCESS_MODE_BIT	(BIT(10))
/* AHB bus */
#define ST90TDS_CONFIG_AHB_PFETCH_BIT	(BIT(11))
#define ST90TDS_CONFIG_CPU_CLK_DIS_BIT	(BIT(12))
/* APB bus */
#define ST90TDS_CONFIG_PFETCH_BIT	(BIT(13))
/* cpu reset */
#define ST90TDS_CONFIG_CPU_RESET_BIT	(BIT(14))
#define ST90TDS_CONFIG_CLEAR_INT_BIT	(BIT(15))

/* For RK960 the IRQ Enable and Ready Bits are in CONFIG register */
#define ST90TDS_CONF_IRQ_RDY_ENABLE	(BIT(16)|BIT(17))

#define dev_to_sdio_func(d)	container_of(d, struct sdio_func, dev)

/* extern */ struct rk960_common;

int cw2000_sdio_readb(struct rk960_common *priv, u32 addr);
int cw2000_sdio_writeb(struct rk960_common *priv, u32 addr, u8 val);

int rk960_data_read(struct rk960_common *priv, void *buf, size_t buf_len);
int rk960_data_write(struct rk960_common *priv,
		     const void *buf, size_t buf_len);

int rk960_reg_read(struct rk960_common *priv, u32 addr,
		   void *buf, size_t buf_len);
int rk960_reg_write(struct rk960_common *priv, u32 addr,
		    const void *buf, size_t buf_len);

int rk960_sdio_set_bus_width(struct rk960_common *priv, int width, int lock);
int rk960_sdio_get_bus_width(struct rk960_common *priv);
int rk960_get_sdio_width_param(void);

#endif /* RK960_HWIO_H_INCLUDED */
