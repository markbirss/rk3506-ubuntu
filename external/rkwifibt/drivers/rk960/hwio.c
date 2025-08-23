/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/types.h>

#include "rk960.h"
#include "hwio.h"
#include "hwbus.h"
#include "debug.h"

#define MAX_RETRY		3

int cw2000_sdio_readb(struct rk960_common *priv, u32 addr)
{
	int val, error;
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);

	sdio_claim_host(func);
	val = sdio_readb(func, addr, &error);
	sdio_release_host(func);

	if (val == 0xff)
		return error;
	else
		return val;
}

int cw2000_sdio_writeb(struct rk960_common *priv, u32 addr, u8 val)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error;

	sdio_claim_host(func);
	sdio_writeb(func, val, addr, &error);
	sdio_release_host(func);

	return error;
}

int rk960_reg_read(struct rk960_common *priv, u32 addr, void *buf,
		   size_t buf_len)
{
	int ret;

	priv->hwbus_ops->lock(priv->hwbus_priv);
	ret = priv->hwbus_ops->hwbus_memcpy_fromio(priv->hwbus_priv,
						   addr, buf, buf_len);
	priv->hwbus_ops->unlock(priv->hwbus_priv);

	return ret;
}

int rk960_reg_write(struct rk960_common *priv, u32 addr, const void *buf,
		    size_t buf_len)
{
	int ret;

	priv->hwbus_ops->lock(priv->hwbus_priv);
	ret = priv->hwbus_ops->hwbus_memcpy_toio(priv->hwbus_priv,
						 addr, buf, buf_len);
	priv->hwbus_ops->unlock(priv->hwbus_priv);

	return ret;
}

int rk960_data_read(struct rk960_common *priv, void *buf, size_t buf_len)
{
#if 1
	int ret;

	ret = rk960_reg_read(priv, 0, buf, buf_len);

	return ret;
#else
	int ret, retry = 1;

	priv->hwbus_ops->lock(priv->hwbus_priv);
	while (retry <= MAX_RETRY) {
		ret = rk960_reg_read(priv, 0, buf, buf_len);
		if (!ret) {
			break;
		} else {
			retry++;
			mdelay(1);
			pr_err("error :[%d]\n", ret);
		}
	}
	priv->hwbus_ops->unlock(priv->hwbus_priv);

	return ret;
#endif
}

int rk960_data_write(struct rk960_common *priv, const void *buf, size_t buf_len)
{
#if 1
	int ret;

	ret = rk960_reg_write(priv, 0, buf, buf_len);

	return ret;
#else
	int ret, retry = 1;

	priv->hwbus_ops->lock(priv->hwbus_priv);
	while (retry <= MAX_RETRY) {
		ret = rk960_reg_write(priv, 0, buf, buf_len);
		if (!ret) {
			break;
		} else {
			retry++;
			mdelay(1);
			RK960_ERROR_IO("error :[%d]\n", ret);
		}
	}
	priv->hwbus_ops->unlock(priv->hwbus_priv);

	return ret;
#endif
}

static int rk960_mmc_io_rw_direct_host(struct mmc_host *host, int write,
				       unsigned fn, unsigned addr, u8 in,
				       u8 * out)
{
	struct mmc_command cmd = { 0 };
	int err;

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;

	cmd.opcode = SD_IO_RW_DIRECT;
	cmd.arg = write ? 0x80000000 : 0x00000000;
	cmd.arg |= fn << 28;
	cmd.arg |= (write && out) ? 0x08000000 : 0x00000000;
	cmd.arg |= addr << 9;
	cmd.arg |= in;
	cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_AC;

	err = mmc_wait_for_cmd(host, &cmd, 0);
	if (err)
		return err;

	{
		if (cmd.resp[0] & R5_ERROR)
			return -EIO;
		if (cmd.resp[0] & R5_FUNCTION_NUMBER)
			return -EINVAL;
		if (cmd.resp[0] & R5_OUT_OF_RANGE)
			return -ERANGE;
	}

	if (out) {
		*out = cmd.resp[0] & 0xFF;
	}

	return 0;
}

static int rk960_sdio_enable_wide(struct mmc_card *card)
{
	int ret;
	u8 ctrl;

	if (card->cccr.low_speed && !card->cccr.wide_bus)
		return 0;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 0, 0, SDIO_CCCR_IF, 0,
					&ctrl);
	if (ret)
		return ret;

	/* set as 4-bit bus width */
	ctrl &= ~SDIO_BUS_WIDTH_MASK;
	ctrl |= SDIO_BUS_WIDTH_4BIT;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 1, 0, SDIO_CCCR_IF, ctrl,
					NULL);
	if (ret)
		return ret;

	return 1;
}

static int rk960_sdio_disable_wide(struct mmc_card *card)
{
	int ret;
	u8 ctrl;

	if (card->cccr.low_speed && !card->cccr.wide_bus)
		return 0;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 0, 0, SDIO_CCCR_IF, 0,
					&ctrl);
	if (ret)
		return ret;

	/* set as 4-bit bus width */
	ctrl &= ~SDIO_BUS_WIDTH_4BIT;
	ctrl |= SDIO_BUS_ASYNC_INT;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 1, 0, SDIO_CCCR_IF, ctrl,
					NULL);
	if (ret)
		return ret;

	return 1;
}

static void rk960_mmc_set_bus_width(struct mmc_host *host, unsigned int width)
{
	host->ios.bus_width = width;
	host->ops->set_ios(host, &host->ios);
}

int rk960_sdio_set_bus_width(struct rk960_common *priv, int width, int lock)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	struct mmc_host *shost = func->card->host;
	int err;

	if (lock)
		sdio_claim_host(func);

        if (width == 4) {
        	err = rk960_sdio_enable_wide(func->card);
        	if (err > 0) {
        		rk960_mmc_set_bus_width(shost, MMC_BUS_WIDTH_4);
        	} else {
        		RK960_ERROR_IO("%s: width %d failed (%d)\n",
                                __func__, width, err);
        	}
        } else if (width == 1) {
        	err = rk960_sdio_disable_wide(func->card);
        	if (err > 0) {
        		rk960_mmc_set_bus_width(shost, MMC_BUS_WIDTH_1);
        	} else {
        		RK960_ERROR_IO("%s: width %d failed (%d)\n",
                                __func__, width, err);
        	}
        }

	if (lock)
		sdio_release_host(func);

	return 0;
}

int rk960_sdio_get_bus_width(struct rk960_common *priv)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	struct mmc_host *shost = func->card->host;

	return 1 << shost->ios.bus_width;
}

// need lock before call
int __rk960_notify(struct rk960_common *priv)
{
	int ret = 0;
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error;
	int val;
	int onetime = 1;	// us
	int count = 500 * 1000;	// wait total (onetime*count) ms  

	sdio_writeb(func, IO_NOTIFY_VAL, IO_NOTIFY_ADDR, &error);

	while (count--) {
		//if (hpriv->fw_error_processing)
		//return -1;
		val = sdio_readb(func, IO_WRITE_REQ_INT_STA, &error);
		if (val == 0) {
			//pr_err("%s: %d\n", __func__, count);
			return 0;	// success
		} else if (val < 0) {
			RK960_ERROR_IO("%s: error %d\n", __func__, val);
			return -1;
		}
		udelay(onetime);
		//pr_info("count = %d, val = %d\n", count, val);
	}

	return ret;
}

int __rk960_irq_enable(struct rk960_common *priv, int enable)
{
	return 0;
}

int __rk960_clear_irq(struct rk960_common *priv)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error = 0;

	sdio_claim_host(func);
	sdio_writeb(func, IO_INT_CLR_IRQ_VAL, IO_INT_ADDR, &error);	//IO_INT_CLR_IRQ_VAL
	//__rk960_notify(priv);
	sdio_release_host(func);

	return error;
}

int __rk960_wakeup_device(struct rk960_common *priv, int wake)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error = 0;
	int value = wake ? IO_NOTIFY_WAKEUP : IO_NOTIFY_SLEEP;

	sdio_claim_host(func);
	sdio_writeb(func, value, IO_NOTIFY_ADDR, &error);
	sdio_release_host(func);

	return error;
}

int __rk960_jtag_enable(struct rk960_common *priv)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error = 0;
	int value = IO_NOTIFY_JTAG;

	sdio_claim_host(func);
	sdio_writeb(func, value, IO_NOTIFY_ADDR, &error);
	sdio_release_host(func);

	return 0;
}

int __rk960_reset_device(struct rk960_common *priv)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error = 0;
	int value = IO_NOTIFY_RESET;

	sdio_claim_host(func);
	sdio_writeb(func, value, IO_NOTIFY_ADDR, &error);
	sdio_release_host(func);

	return 0;
}

int __rk960_notify_device_pwrdown(struct rk960_common *priv)
{
	struct sdio_func *func = dev_to_sdio_func(priv->pdev);
	int error = 0;
	int value = IO_NOTIFY_PWRDOWN;

	sdio_claim_host(func);
	sdio_writeb(func, value, IO_NOTIFY_ADDR, &error);
	sdio_release_host(func);

	return 0;
}
