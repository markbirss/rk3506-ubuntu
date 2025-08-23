/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio.h>
#include <net/mac80211.h>

#include "rk960.h"
#include "hwbus.h"
#include "debug.h"

static int pwr_ctrl = 0;
module_param(pwr_ctrl, int, 0644);
MODULE_PARM_DESC(pwr_ctrl, "power control");

struct rk960_platform_data_spi {
	u8 spi_bits_per_word;	/* REQUIRED */
	u16 ref_clk;		/* REQUIRED (in KHz) */

	/* All others are optional */
	bool have_5ghz;
	int reset;		/* GPIO to RSTn signal (0 disables) */
	int powerup;		/* GPIO to POWERUP signal (0 disables) */
	int (*power_ctrl) (const struct rk960_platform_data_spi * pdata, bool enable);	/* Control 3v3 / 1v8 supply */
	int (*clk_ctrl) (const struct rk960_platform_data_spi * pdata, bool enable);	/* Control CLK32K */
	const u8 *macaddr;	/* if NULL, use rk960_mac_template module parameter */
	const char *sdd_file;	/* if NULL, will use default for detected hw type */
};

struct rk960_platform_data_sdio {
	u16 ref_clk;		/* REQUIRED (in KHz) */

	/* All others are optional */
	bool have_5ghz;
	bool no_nptb;		/* SDIO hardware does not support non-power-of-2-blocksizes */
	int reset;		/* GPIO to RSTn signal (0 disables) */
	int powerup;		/* GPIO to POWERUP signal (0 disables) */
	int irq;		/* IRQ line or 0 to use SDIO IRQ */
	int (*power_ctrl) (const struct rk960_platform_data_sdio * pdata, bool enable);	/* Control 3v3 / 1v8 supply */
	int (*clk_ctrl) (const struct rk960_platform_data_sdio * pdata, bool enable);	/* Control CLK32K */
	const u8 *macaddr;	/* if NULL, use rk960_mac_template module parameter */
	const char *sdd_file;	/* if NULL, will use default for detected hw type */
};
void __init rk960_sdio_set_platform_data(struct rk960_platform_data_sdio
					 *pdata);

#include "hwio.h"

MODULE_AUTHOR("Rockchips");
MODULE_DESCRIPTION("Rockchips RK960 Wireless Lan Driver");
MODULE_LICENSE("GPL");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#endif

/* Default platform data for Sagrad modules */
static struct rk960_platform_data_sdio rk960_platform_data = {
	.ref_clk = 38400,
	.have_5ghz = false,
	.irq = 1,
	.no_nptb = 0,
	.sdd_file = "sdd_sagrad_1091_1098.bin",
};

/* Allow platform data to be overridden */
static struct rk960_platform_data_sdio *global_plat_data = &rk960_platform_data;

void __init rk960_sdio_set_platform_data(struct rk960_platform_data_sdio *pdata)
{
	global_plat_data = pdata;
}

struct hwbus_priv {
	struct sdio_func *func;
	struct rk960_common *core;
	const struct rk960_platform_data_sdio *pdata;
	int irq_req;
};

static const struct sdio_device_id rk960_sdio_ids[] = {
	{SDIO_DEVICE(RK960_MANUFACTURER, RK960_DEVICE_ID)},
	{SDIO_DEVICE(RK960_MANUFACTURER, RK960_DEVICE_ID_C)},
	{SDIO_DEVICE(RK960_MANUFACTURER, RK960_DEVICE_ID_D)},
	{},
};

/* hwbus_ops implemetation */

static int rk960_sdio_memcpy_fromio(struct hwbus_priv *self,
				    unsigned int addr, void *dst, int count)
{
	int ret;

	//RK960_DEBUG_IO("%s: addr %x count %d\n", __func__, addr, count);
	ret = sdio_memcpy_fromio(self->func, dst, addr, count);
#if 0
	print_hex_dump(KERN_INFO, " ",
		       DUMP_PREFIX_NONE, 16, 1,
		       dst, /*(count > 512) ? 512 : */ count, 1);
#endif
	return ret;
}

static int rk960_sdio_memcpy_toio(struct hwbus_priv *self,
				  unsigned int addr, const void *src, int count)
{
	//RK960_DEBUG_IO("%s: addr %x count %d\n", __func__, addr, count);
#if 0
	print_hex_dump(KERN_INFO, " ",
		       DUMP_PREFIX_NONE, 16, 1,
		       src, (count > 512) ? 512 : count, 1);
#endif
	return sdio_memcpy_toio(self->func, addr, (void *)src, count);
}

static void rk960_sdio_lock(struct hwbus_priv *self)
{
	sdio_claim_host(self->func);
}

static void rk960_sdio_unlock(struct hwbus_priv *self)
{
	sdio_release_host(self->func);
}

static void rk960_sdio_irq_handler(struct sdio_func *func)
{
	struct hwbus_priv *self = sdio_get_drvdata(func);

	/* note:  sdio_host already claimed here. */
	if (self->core)
		rk960_irq_handler(self->core);
}

static irqreturn_t rk960_gpio_hardirq(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}

static irqreturn_t rk960_gpio_irq(int irq, void *dev_id)
{
	struct hwbus_priv *self = dev_id;

	if (self->core) {
		rk960_irq_handler(self->core);
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
}

int rk960_sdio_device_init(struct hwbus_priv *self)
{
	int error;
	unsigned char value;
	struct sdio_func *func = self->func;

	RK960_DEBUG_IO("%s\n", __func__);

	sdio_claim_host(func);

	/* Interrupt Enable for Function x */
	sdio_f0_writeb(func, 0x07, 0x04, &error);
	if (error) {
		goto fail;
	}
	/* Default is GPIO interrupt, if it's "0", DATA1 interrupt */
	sdio_f0_writeb(func, 0x80, 0x16, &error);
	if (error)
		goto fail;

	/* Block Size for Function 0 */
	error = sdio_set_block_size(func, RK960_SDIO_BLOCK_SIZE);
	if (error) {
		goto fail;
	}

	/* It can generate an interrupt to host */
	sdio_writeb(func, 0x02, 34, &error);
	if (error) {
		goto fail;
	}

	sdio_writeb(func, 0x02, 33, &error);
	if (error) {
		goto fail;
	}

	/* clear interrupt to host */
	value = sdio_readb(func, 32, &error);
	if (error) {
		goto fail;
	}

	sdio_writeb(func, value, 32, &error);
	if (error) {
		goto fail;
	}

	sdio_release_host(func);

	RK960_INFO_IO("%s success.\n", __func__);
	return 0;

fail:
	sdio_release_host(func);

	RK960_ERROR_IO("%s failed.\n", __func__);
	return error;
}

static int rk960_request_irq(struct hwbus_priv *self)
{
	int ret;

	RK960_DEBUG_IO("%s\n", __func__);

	ret = enable_irq_wake(self->pdata->irq);
	if (WARN_ON(ret))
		goto err;

	self->irq_req = 1;
	/* Request the IRQ */
	ret = request_threaded_irq(self->pdata->irq, rk960_gpio_hardirq,
				   rk960_gpio_irq,
				   IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND,
				   "rk960_wlan_irq", self);

	if (WARN_ON(ret))
		goto err;

	return 0;

err:
	RK960_ERROR_IO("request_irq failed\n");
	return ret;
}

int rk960_sdio_irq_subscribe(struct hwbus_priv *self)
{
	int ret = 0;

	RK960_DEBUG_IO("%s\n", __func__);

	sdio_claim_host(self->func);
	if (self->pdata->irq)
		ret = rk960_request_irq(self);
	else
		ret = sdio_claim_irq(self->func, rk960_sdio_irq_handler);

	sdio_release_host(self->func);
	return ret;
}

static int rk960_sdio_irq_unsubscribe(struct hwbus_priv *self)
{
	int ret = 0;

	RK960_DEBUG_IO("%s\n", __func__);

	if (self->pdata->irq) {
		if (self->irq_req) {
			disable_irq_wake(self->pdata->irq);
			free_irq(self->pdata->irq, self);
		}
	} else {
		sdio_claim_host(self->func);
		ret = sdio_release_irq(self->func);
		sdio_release_host(self->func);
	}
	return ret;
}

static int rk960_sdio_off(const struct rk960_platform_data_sdio *pdata)
{
	if (!pwr_ctrl)
		return 0;
#ifdef HW_RESET
	rockchip_wifi_set_carddetect(0);
	mdelay(10);
	rockchip_wifi_power(0);
#endif
	return 0;
}

static int rk960_sdio_on(const struct rk960_platform_data_sdio *pdata)
{
	if (!pwr_ctrl)
		return 0;
#ifdef HW_RESET
	rockchip_wifi_power(0);
	mdelay(10);
	rockchip_wifi_power(1);
	mdelay(10);
	rockchip_wifi_set_carddetect(1);
#endif
	return 0;
}

static size_t rk960_sdio_align_size(struct hwbus_priv *self, size_t size)
{
#if 0
	if (self->pdata->no_nptb)
		size = round_up(size, RK960_SDIO_BLOCK_SIZE);
	else
		size = sdio_align_size(self->func, size);
#else
#if USE_SDIO_TXRX_BLKSIZE_ALIGN
	size = round_up(size, RK960_SDIO_BLOCK_SIZE);
#else
	size = round_up(size, 4);
#endif
#endif
	return size;
}

static int rk960_sdio_pm(struct hwbus_priv *self, bool suspend)
{
	int ret = 0;

	if (self->pdata->irq)
		ret = irq_set_irq_wake(self->pdata->irq, suspend);
	return ret;
}

static struct hwbus_ops rk960_sdio_hwbus_ops = {
	.hwbus_memcpy_fromio = rk960_sdio_memcpy_fromio,
	.hwbus_memcpy_toio = rk960_sdio_memcpy_toio,
	.lock = rk960_sdio_lock,
	.unlock = rk960_sdio_unlock,
	.align_size = rk960_sdio_align_size,
	.power_mgmt = rk960_sdio_pm,
};

/* Probe Function to be called by SDIO stack when device is discovered */
static int rk960_sdio_probe(struct sdio_func *func,
			    const struct sdio_device_id *id)
{
	struct hwbus_priv *self;
	int status;

	RK960_INFO_IO("%s: func num %d chip id %x\n",
		      __func__, func->num, id->device);

	/* We are only able to handle the wlan function */
	if (func->num != 0x01)
		return -ENODEV;

	self = kzalloc(sizeof(*self), GFP_KERNEL);
	if (!self) {
		RK960_ERROR_IO("Can't allocate SDIO hwbus_priv.\n");
		return -ENOMEM;
	}

	func->card->quirks |= MMC_QUIRK_LENIENT_FN0;
	global_plat_data->irq = rockchip_wifi_get_oob_irq();
	self->pdata = global_plat_data;	/* FIXME */
	self->func = func;
	sdio_set_drvdata(func, self);
	sdio_claim_host(func);
	sdio_enable_func(func);
	sdio_release_host(func);

	status = rk960_core_probe(&rk960_sdio_hwbus_ops,
				  self, &func->dev, id->device, &self->core,
				  self->pdata->ref_clk,
				  self->pdata->macaddr,
				  self->pdata->sdd_file,
				  self->pdata->have_5ghz);
	if (status) {
		rk960_sdio_irq_unsubscribe(self);
		sdio_claim_host(func);
		sdio_disable_func(func);
		sdio_release_host(func);
		sdio_set_drvdata(func, NULL);
		kfree(self);
	}

	self->core->hwbus_priv = self;
	return status;
}

/* Disconnect Function to be called by SDIO stack when
 * device is disconnected
 */
static void rk960_sdio_disconnect(struct sdio_func *func)
{
	struct hwbus_priv *self = sdio_get_drvdata(func);

	RK960_INFO_IO("%s\n", __func__);

	if (self) {
		if (self->core) {
			rk960_core_release(self->core);
			rk960_sdio_irq_unsubscribe(self);
			self->core = NULL;
		}
		sdio_claim_host(func);
		sdio_disable_func(func);
		sdio_release_host(func);
		sdio_set_drvdata(func, NULL);
		kfree(self);
	}
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

static int rk960_mmc_select_card(struct mmc_host *host, struct mmc_card *card)
{
	int err;
	struct mmc_command cmd = { 0 };

	cmd.opcode = MMC_SELECT_CARD;

	if (card) {
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	} else {
		cmd.arg = 0;
		cmd.flags = MMC_RSP_NONE | MMC_CMD_AC;
	}

	err = mmc_wait_for_cmd(host, &cmd, 3);
	if (err)
		return err;

	return 0;
}

static int rk960_sdio_reset(struct mmc_card *card)
{
	int ret;
	u8 abort;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 0, 0, SDIO_CCCR_ABORT, 0,
					&abort);
	if (ret)
		abort = 0x08;
	else
		abort |= 0x08;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 1, 0, SDIO_CCCR_ABORT,
					abort, NULL);
	return ret;
}

static int rk960_mmc_go_idle(struct mmc_host *host)
{
	int err;
	struct mmc_command cmd = { 0 };

	cmd.opcode = MMC_GO_IDLE_STATE;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_NONE | MMC_CMD_BC;

	err = mmc_wait_for_cmd(host, &cmd, 0);

	mdelay(1);

	host->use_spi_crc = 0;

	return err;
}

int rk960_mmc_send_io_op_cond(struct mmc_host *host, u32 ocr, u32 * rocr)
{
	struct mmc_command cmd = { 0 };
	int i, err = 0;

	cmd.opcode = SD_IO_SEND_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_SPI_R4 | MMC_RSP_R4 | MMC_CMD_BCR;

	for (i = 100; i; i--) {
		err = mmc_wait_for_cmd(host, &cmd, 3);
		if (err)
			break;

		/* if we're just probing, do a single pass */
		if (ocr == 0)
			break;

		/* otherwise wait until reset completes */
		{
			if (cmd.resp[0] & MMC_CARD_BUSY)
				break;
		}

		err = -ETIMEDOUT;

		mdelay(10);
	}

	return err;
}

int rk960_mmc_send_relative_addr(struct mmc_host *host, unsigned int *rca)
{
	int err;
	struct mmc_command cmd = { 0 };

	cmd.opcode = SD_SEND_RELATIVE_ADDR;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R6 | MMC_CMD_BCR;

	err = mmc_wait_for_cmd(host, &cmd, 3);
	if (err)
		return err;

	*rca = cmd.resp[0] >> 16;

	return 0;
}

static int rk960_mmc_sdio_switch_hs(struct mmc_card *card, int enable)
{
	int ret;
	u8 speed;

	if (!(card->host->caps & MMC_CAP_SD_HIGHSPEED))
		return 0;

	if (!card->cccr.high_speed)
		return 0;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 0, 0, SDIO_CCCR_SPEED, 0,
					&speed);
	if (ret)
		return ret;

	if (enable)
		speed |= SDIO_SPEED_EHS;
	else
		speed &= ~SDIO_SPEED_EHS;

	ret =
	    rk960_mmc_io_rw_direct_host(card->host, 1, 0, SDIO_CCCR_SPEED,
					speed, NULL);
	if (ret)
		return ret;

	return 1;
}

static int rk960_sdio_enable_hs(struct mmc_card *card)
{
	int ret;

	ret = rk960_mmc_sdio_switch_hs(card, true);
	/*if (ret <= 0 || card->type == MMC_TYPE_SDIO)
	   return ret;

	   ret = mmc_sd_switch_hs(card);
	   if (ret <= 0)
	   mmc_sdio_switch_hs(card, false); */

	return ret;
}

static void rk960_mmc_power_up(struct mmc_host *host, u32 ocr)
{

	host->ios.chip_select = MMC_CS_DONTCARE;
	host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
	host->ios.power_mode = MMC_POWER_UP;
	host->ios.bus_width = MMC_BUS_WIDTH_1;
	host->ios.timing = MMC_TIMING_LEGACY;
	host->ops->set_ios(host, &host->ios);

	mdelay(10);

	host->ios.clock = host->f_init;

	host->ios.power_mode = MMC_POWER_ON;
	host->ops->set_ios(host, &host->ios);

	mdelay(10);
}

static void rk960_mmc_set_clock(struct mmc_host *host, unsigned int hz)
{
	if (hz > host->f_max)
		hz = host->f_max;

	host->ios.clock = hz;
	host->ops->set_ios(host, &host->ios);
}

void rk960_sdio_set_clock(struct rk960_common *hw_priv, int hz)
{
	struct sdio_func *func = (struct sdio_func *)hw_priv->hwbus_priv->func;
	struct mmc_host *shost = func->card->host;

	sdio_claim_host(func);
	rk960_mmc_set_clock(shost, hz);
	sdio_release_host(func);
}

int rk960_sdio_recovery_init(struct rk960_common *hw_priv)
{
	struct sdio_func *func = (struct sdio_func *)hw_priv->hwbus_priv->func;
	struct mmc_host *shost = func->card->host;
	int err;
	u32 rocr, ocr, rca;

	RK960_INFO_FWREC("%s\n", __func__);

	sdio_claim_host(func);

	shost->ios.power_mode = MMC_POWER_OFF;
	rk960_mmc_power_up(shost, 1);

	err = rk960_sdio_reset(func->card);
	/*if (err) {
	   RPU_ERROR_ROCOVERY("rk915_sdio_reset failed (%d)\n", err);
	   goto err_out;
	   } */

	err = rk960_mmc_go_idle(shost);
	if (err) {
		RK960_ERROR_FWREC("rk960_mmc_go_idle failed (%d)\n", err);
		goto err_out;
	}

	ocr = 0;
	err = rk960_mmc_send_io_op_cond(shost, ocr, &rocr);
	if (err) {
		RK960_ERROR_FWREC("rk960_mmc_send_io_op_cond1 failed (%d)\n",
				  err);
		goto err_out;
	}

	ocr = 0x1800000;
	err = rk960_mmc_send_io_op_cond(shost, ocr, &rocr);
	if (err) {
		RK960_ERROR_FWREC("rk960_mmc_send_io_op_cond2 failed (%d)\n",
				  err);
		goto err_out;
	}

	err = rk960_mmc_send_relative_addr(shost, &rca);
	if (err) {
		RK960_ERROR_FWREC("rk960_mmc_send_relative_addr failed (%d)\n",
				  err);
		goto err_out;
	}

	err = rk960_mmc_select_card(shost, func->card);
	if (err) {
		RK960_ERROR_FWREC("rk960_mmc_select_card failed (%d)\n", err);
		goto err_out;
	}

	err = rk960_sdio_enable_hs(func->card);
	if (err <= 0) {
		RK960_ERROR_FWREC("rk960_sdio_enable_hs failed (%d)\n", err);
		goto err_out;
	}

	rk960_mmc_set_clock(shost, 50000000);

        if (rk960_get_sdio_width_param() == 4) {
                rk960_sdio_set_bus_width(hw_priv, 4, 0);
        }

	err = sdio_set_block_size(func, RK960_SDIO_BLOCK_SIZE);
	if (err) {
		RK960_ERROR_FWREC("sdio_set_block_size failed (%d)\n", err);
		goto err_out;
	}

	err = sdio_enable_func(func);
	if (err) {
		RK960_ERROR_FWREC("sdio_enable_func failed (%d)\n", err);
		goto err_out;
	}

err_out:
	sdio_release_host(func);
	return err;
}

#ifdef CONFIG_PM
static int rk960_sdio_suspend(struct device *dev)
{
	int ret;
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct hwbus_priv *self = sdio_get_drvdata(func);

	RK960_INFO_IO("%s\n", __func__);

	if (!rk960_can_suspend(self->core))
		return -EAGAIN;

	if (rk960_suspend(self->core) != 0)
		return -EAGAIN;

	//rk960_device_wakeup(self->core, 0);

	/* Notify SDIO that RK960 will remain powered during suspend */
	ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);
	if (ret)
		RK960_ERROR_IO("Error setting SDIO pm flags: %i\n", ret);

	return ret;
}

static int rk960_sdio_resume(struct device *dev)
{
	struct sdio_func *func = dev_to_sdio_func(dev);
	struct hwbus_priv *self = sdio_get_drvdata(func);

	RK960_INFO_IO("%s\n", __func__);

	/*rk960_device_wakeup(self->core, 1); */

	rk960_resume(self->core);

	return 0;
}

static const struct dev_pm_ops rk960_sdio_pm_ops = {
	.suspend = rk960_sdio_suspend,
	.resume = rk960_sdio_resume,
};
#endif

static struct sdio_driver sdio_driver = {
	.name = "rk960_wlan_sdio",
	.id_table = rk960_sdio_ids,
	.probe = rk960_sdio_probe,
	.remove = rk960_sdio_disconnect,
#ifdef CONFIG_PM
	.drv = {
		.pm = &rk960_sdio_pm_ops,
		}
#endif
};

/* Init Module function -> Called by insmod */
static int __init rk960_sdio_init(void)
{
	const struct rk960_platform_data_sdio *pdata;
	int ret;

	rk960_debug_level_init();

	RK960_INFO_IO("WiFi Driver: version: %s\n", RK960_VERSION);
	RK960_INFO_IO("Build time: %s %s\n", __DATE__, __TIME__);

	RK960_INFO_IO("%s\n", __func__);

	/* FIXME -- this won't support multiple devices */
	pdata = global_plat_data;

	if (rk960_sdio_on(pdata)) {
		//ret = -1;
		//goto err;
	}

	ret = sdio_register_driver(&sdio_driver);
	if (ret)
		goto err;

	return 0;

err:
	rk960_sdio_off(pdata);
	return ret;
}

/* Called at Driver Unloading */
static void __exit rk960_sdio_exit(void)
{
	const struct rk960_platform_data_sdio *pdata;

	RK960_INFO_IO("%s\n", __func__);

	/* FIXME -- this won't support multiple devices */
	pdata = global_plat_data;
	sdio_unregister_driver(&sdio_driver);
	rk960_sdio_off(pdata);
}

module_init(rk960_sdio_init);
module_exit(rk960_sdio_exit);
