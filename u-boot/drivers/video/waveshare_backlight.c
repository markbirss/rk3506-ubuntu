/*
 * Copyright (c) 2016 Google, Inc
 * Written by luckfox_eng33 <eng33@luckfox.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <backlight.h>
#include <asm/gpio.h>
#include <power/regulator.h>

#define REG_LCD                  0x95
#define WAVESHARE_REGSET         0x96

struct waveshare_backlight_priv {
	struct udevice *dev;
	uint def_brightness;
	uint max_brightness;
};

int backlight_i2c_write(struct waveshare_backlight_priv *priv_data, u8 reg, u8 val)
{
	int ret;
	u8 buf[2];
	struct i2c_msg msg;
	struct dm_i2c_chip *chip = dev_get_parent_platdata(priv_data->dev);

	buf[0] = reg;
	buf[1] = val;
	msg.addr = chip->chip_addr;
	msg.flags = 0;
	msg.len = 2;
	msg.buf = buf;
	// printf("%s: %s: chip_addr: 0x%x \n", __FILE__, __func__, msg.addr);

	ret = dm_i2c_xfer(priv_data->dev, &msg, 1);
	if (ret) {
		printf("%s: %s: waveshare i2c write failed: %d \n", __FILE__, __func__, ret);
		return ret;
	}

	return 0;
}

static int waveshare_backlight_enable(struct udevice *dev)
{
	struct waveshare_backlight_priv *priv_data = dev_get_priv(dev);
	// printf("%s: %s: brightness: %d \n", __FILE__, __func__, priv_data->def_brightness);
	backlight_i2c_write(priv_data, WAVESHARE_REGSET, priv_data->def_brightness);
	return 0;
}

static int waveshare_backlight_disable(struct udevice *dev)
{
	struct waveshare_backlight_priv *priv_data = dev_get_priv(dev);
	backlight_i2c_write(priv_data, WAVESHARE_REGSET, 0);
	return 0;
}

static int waveshare_backlight_ofdata_to_platdata(struct udevice *dev)
{
	return 0;
}

static int waveshare_backlight_probe(struct udevice *dev)
{
	struct waveshare_backlight_priv *waveshare_backlight = dev_get_priv(dev);
	u32 brightness = 0;

	waveshare_backlight->dev = dev;

	brightness = dev_read_u32_default(dev, "max-brightness-level", 255);
	waveshare_backlight->max_brightness = brightness;
	printf("%s: %s: max brightness: %d \n", __FILE__, __func__, waveshare_backlight->max_brightness);

	brightness = dev_read_u32_default(dev, "default-brightness-level", -1);
	if (brightness && brightness <= waveshare_backlight->max_brightness) {
		waveshare_backlight->def_brightness = brightness;
	}
	else {
		waveshare_backlight->def_brightness = waveshare_backlight->max_brightness;
		printf("%s: %s: read 'default-brightness-level' failed, ret = %d \n", __FILE__, __func__, brightness);
	}
	// printf("%s: %s: default brightness: %d \n", __FILE__, __func__, waveshare_backlight->def_brightness);

	backlight_i2c_write(waveshare_backlight, REG_LCD, 0x11);
	udelay(10000);
	backlight_i2c_write(waveshare_backlight, REG_LCD, 0x17);

	return 0;
}

static const struct backlight_ops waveshare_backlight_ops = {
	.enable	= waveshare_backlight_enable,
	.disable = waveshare_backlight_disable,
};

static const struct udevice_id waveshare_backlight_ids[] = {
	{ .compatible = "waveshare,dsi-backlight" },
	{ }
};

U_BOOT_DRIVER(waveshare_backlight) = {
	.name	= "waveshare_backlight",
	.id	= UCLASS_PANEL_BACKLIGHT,
	.of_match = waveshare_backlight_ids,
	.ops	= &waveshare_backlight_ops,
	.ofdata_to_platdata	= waveshare_backlight_ofdata_to_platdata,
	.probe		= waveshare_backlight_probe,
	.priv_auto_alloc_size	= sizeof(struct waveshare_backlight_priv),
};
