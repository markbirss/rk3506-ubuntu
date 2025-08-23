/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_HWBUS_H
#define RK960_HWBUS_H

/* only one of the following two can enable */
#define USE_SDIO_RX_BURST 0
#define USE_SDIO_RX_BURST_MULTI 1

#define USE_SDIO_TX_BURST_MULTI 0

#define USE_SDIO_TXRX_BLKSIZE_ALIGN 1
#define USE_SDIO_TX_CONTINUE 1
#define USE_SDIO_RX_CONTINUE 0

#if USE_SDIO_TXRX_BLKSIZE_ALIGN
#define RK960_SDIO_BLOCK_SIZE              128	// 128, 256, 512
#if (RK960_SDIO_BLOCK_SIZE == 512)
#define RK960_SDIO_TX_MSG_SIZE            2048
#elif (RK960_SDIO_BLOCK_SIZE == 256)
#define RK960_SDIO_TX_MSG_SIZE            1792
#elif (RK960_SDIO_BLOCK_SIZE == 128)
#define RK960_SDIO_TX_MSG_SIZE            1664
#endif
#else
#define RK960_SDIO_BLOCK_SIZE              512
#define RK960_SDIO_TX_MSG_SIZE            1632
#endif

#define MAX_SDIO_BURST_PTKS     16
#define EFFECTIVE_RX_BUF_SIZE   (MAX_SDIO_BURST_PTKS*RK960_SDIO_TX_MSG_SIZE)
#define EFFECTIVE_TX_BUF_SIZE   (MAX_SDIO_BURST_PTKS*RK960_SDIO_TX_MSG_SIZE/2)

struct hwbus_priv;

void rk960_irq_handler(struct rk960_common *priv);

/* This MUST be wrapped with hwbus_ops->lock/unlock! */
int __rk960_irq_enable(struct rk960_common *priv, int enable);

int __rk960_clear_irq(struct rk960_common *priv);
int __rk960_wakeup_device(struct rk960_common *priv, int wake);
int __rk960_jtag_enable(struct rk960_common *priv);
int __rk960_reset_device(struct rk960_common *priv);
int __rk960_notify_device_pwrdown(struct rk960_common *priv);

int rk960_sdio_irq_subscribe(struct hwbus_priv *self);

int rk960_sdio_device_init(struct hwbus_priv *self);
int rk960_sdio_recovery_init(struct rk960_common *hw_priv);

struct hwbus_ops {
	int (*hwbus_memcpy_fromio) (struct hwbus_priv * self, unsigned int addr,
				    void *dst, int count);
	int (*hwbus_memcpy_toio) (struct hwbus_priv * self, unsigned int addr,
				  const void *src, int count);
	void (*lock) (struct hwbus_priv * self);
	void (*unlock) (struct hwbus_priv * self);
	 size_t(*align_size) (struct hwbus_priv * self, size_t size);
	int (*power_mgmt) (struct hwbus_priv * self, bool suspend);
};

#endif /* RK960_HWBUS_H */
