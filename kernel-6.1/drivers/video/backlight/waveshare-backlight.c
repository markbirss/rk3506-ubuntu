// SPDX-License-Identifier: GPL-2.0-only

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/fb.h>
#include <linux/i2c.h>
#include <linux/media-bus-format.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_graph.h>
#include <linux/pm.h>

#define MAX_BRIGHTNESS		0xff
#define DEF_BRIGHTNESS		0x80

#define RASPI_MODE		0x01
#define WS_RASPI_MODE		0x02
#define WS_MODE 		0x03

#define REG_LCD 		0x95
#define REG_PWM 		0x96
#define REG_SIZE		0x97

#define CFG_VCC_EN		BIT(4)
#define CFG_BL_EN		BIT(2)
#define CFG_LCD_RST		BIT(1)
#define CFG_LCD_PWR		BIT(0)

static int g_panel_mode = 0;

struct waveshare_platform_data {
	const char *name;
	u32 min_brightness;
	u32 def_brightness;
	u32 max_brightness;
};

struct waveshare_bl_data {
	struct device *dev;
	struct i2c_client *i2c;
	struct backlight_device *bl;
	struct waveshare_platform_data *pdata;
};

static int ws_set_brightness(struct waveshare_bl_data *data, u32 brightness)
{
	int ret;
	u8 val;

	if (data->pdata->min_brightness > 0 && brightness > 0 &&
		brightness < data->pdata->min_brightness)
		brightness = data->pdata->min_brightness;

	if (data->pdata->max_brightness > 0 && brightness > 0 &&
		brightness > data->pdata->max_brightness)
		brightness = data->pdata->max_brightness;

	if (g_panel_mode == RASPI_MODE) {
		val = brightness;

		ret = i2c_smbus_write_byte_data(data->i2c, 0x86, val);
		if (ret) {
			dev_err(data->dev, "I2C write failed: %d\n", ret);
			return ret;
		}

	} else if (g_panel_mode == WS_RASPI_MODE) {
		val = 0xff - brightness;
		ret = i2c_smbus_write_byte_data(data->i2c, 0xab, val);
		if (ret) {
			dev_err(data->dev, "I2C write failed: %d\n", ret);
			return ret;
		}

		ret = i2c_smbus_write_byte_data(data->i2c, 0xaa, 0x01);
		if (ret) {
			dev_err(data->dev, "I2C write failed: %d\n", ret);
			return ret;
		}
	} else if (g_panel_mode == WS_MODE) {
		val = brightness;
		unsigned char read_data, write_data;

		read_data = i2c_smbus_read_byte_data(data->i2c, REG_LCD);
		if (read_data < 0) {
			return read_data;
		}

		if (read_data & CFG_BL_EN) {
			if (val == 0) {
				write_data = read_data & ~CFG_BL_EN;
				ret = i2c_smbus_write_byte_data(data->i2c, REG_LCD, write_data);
				if (ret) {
					dev_err(data->dev, "I2C write failed: %d\n", ret);
					return ret;
				}
			}
		} else {
			if (val > 0) {
				write_data = read_data | CFG_BL_EN;
				ret = i2c_smbus_write_byte_data(data->i2c, REG_LCD, write_data);
				if (ret) {
					dev_err(data->dev, "I2C write failed: %d\n", ret);
					return ret;
				}
			}
		}

		ret = i2c_smbus_write_byte_data(data->i2c, REG_PWM ,val);
		if (ret) {
			dev_err(data->dev, "I2C write failed: %d\n", ret);
			return ret;
		}
	}

	return 0;
}

static int ws_bl_update_status(struct backlight_device *bl)
{
	struct waveshare_bl_data *data = bl_get_data(bl);
	u32 brightness = bl->props.brightness;
	int ret;

	ret = ws_set_brightness(data, brightness);
	if (ret)
		return ret;

	return 0;
}

static const struct backlight_ops ws_bl_ops = {
	.update_status = ws_bl_update_status,
};

static int ws_create_backlight(struct waveshare_bl_data *data)
{
	struct backlight_properties *props;
	const char *name = data->pdata->name ? : "waveshare_bl";

	props = devm_kzalloc(data->dev, sizeof(*props), GFP_KERNEL);
	if (!props)
		return -ENOMEM;

	props->type = BACKLIGHT_PLATFORM;
	props->max_brightness = data->pdata->max_brightness;

	if (data->pdata->def_brightness > props->max_brightness)
		data->pdata->def_brightness = props->max_brightness;

	props->brightness = data->pdata->def_brightness;

	data->bl = devm_backlight_device_register(data->dev, name, data->dev, data,
					&ws_bl_ops, props);

	return PTR_ERR_OR_ZERO(data->bl);
}

static void ws_parse_dt(struct waveshare_bl_data *data)
{
	struct device *dev = data->dev;
	struct device_node *node = dev->of_node;
	int ret;

	if(!node) {
		return;
	}

	ret = of_property_read_string(node, "label", &data->pdata->name);
	if (ret < 0)
		data->pdata->name = NULL;

	/* Get Brightness Level */
	ret = of_property_read_u32(data->dev->of_node, "default-brightness-level",
				   &data->pdata->def_brightness);
	if (ret) {
		dev_err(data->dev, "could not get default-brightness-level\n");
		data->pdata->def_brightness = DEF_BRIGHTNESS;
	}

	ret = of_property_read_u32(data->dev->of_node, "max-brightness-level",
				   &data->pdata->max_brightness);
	if (ret) {
		dev_err(data->dev, "could not get max-brightness-level\n");
		data->pdata->max_brightness = MAX_BRIGHTNESS;
	}

	ret = of_property_read_u32(data->dev->of_node, "min-brightness-level",
				   &data->pdata->min_brightness);
	if (ret) {
		dev_err(data->dev, "could not get min-brightness-level,default 0\n");
		data->pdata->min_brightness = 0;
	}
}

static int ws_backlight_probe(struct i2c_client *i2c,const struct i2c_device_id *id)
{
	struct waveshare_bl_data *data;
	int ret;

	data = devm_kzalloc(&i2c->dev, sizeof(*data), GFP_KERNEL);
	if(!data)
	return -ENOMEM;

	data->i2c = i2c;
	data->dev = &i2c->dev;
	data->pdata = dev_get_platdata(&i2c->dev);

	/* Check ID */
	ret = i2c_smbus_read_byte_data(i2c, 0x80);
	if (ret < 0) {
		goto probe_err;
	}
	switch (ret) {
	case 0xde: /* ver 1 */
	case 0xc3: /* ver 2*/
		g_panel_mode = RASPI_MODE;
		break;
	default:
		g_panel_mode = WS_RASPI_MODE;
	}

	ret = i2c_smbus_read_byte_data(i2c, REG_SIZE);
	if (ret < 0) {
		goto probe_err;
	}
	switch (ret) {
	case 0x46:
	case 0x65: /* 10inch1 */
		g_panel_mode = WS_MODE;
	default:
		break;
	}

	/* Init Screen */
	if (g_panel_mode == RASPI_MODE) {
		ret = i2c_smbus_write_byte_data(i2c, 0x85, 0x00);
		if (ret)
			goto probe_err;
		msleep(800);

		ret = i2c_smbus_write_byte_data(i2c, 0x85, 0x01);
		if (ret)
			goto probe_err;
		msleep(800);

		ret = i2c_smbus_write_byte_data(i2c, 0x81, 0x04);
		if (ret)
			goto probe_err;
	} else if (g_panel_mode == WS_RASPI_MODE) {
		ret = i2c_smbus_write_byte_data(i2c, 0xc0, 0x01);
		if (ret)
			goto probe_err;

		ret = i2c_smbus_write_byte_data(i2c, 0xc2, 0x01);
		if (ret)
			goto probe_err;

		ret = i2c_smbus_write_byte_data(i2c, 0xac, 0x01);
		if (ret)
			goto probe_err;
	} else if (g_panel_mode == WS_MODE) {
		ret = i2c_smbus_read_byte_data(i2c, REG_LCD);
		if (ret < 0) {
			goto probe_err;
		}

		if (ret & CFG_LCD_PWR) {
			dev_info(data->dev, "LCD is enabled\n");
		} else {
			ret = i2c_smbus_write_byte_data(i2c, REG_LCD, 0x11);
			if (ret)
				goto probe_err;
			msleep(800);

			ret = i2c_smbus_write_byte_data(i2c, REG_LCD, 0x17);
			if (ret)
				goto probe_err;
		}
	}

	/* Init Platform Data */
	if(!data->pdata) {
		data->pdata = devm_kzalloc(data->dev, sizeof(*data->pdata), GFP_KERNEL);
		if (!data->pdata)
			return -ENOMEM;

		data->pdata->name = NULL;
		data->pdata->min_brightness = 0;
		data->pdata->def_brightness = DEF_BRIGHTNESS;
		data->pdata->max_brightness = MAX_BRIGHTNESS;

		if (IS_ENABLED(CONFIG_OF))
			ws_parse_dt(data);
	}

	i2c_set_clientdata(i2c, data);

	/* Set Initial Brightness */
	ret = ws_set_brightness(data, data->pdata->def_brightness);
	if (ret)
		goto probe_err;

	/* Create Backlight */
	ret = ws_create_backlight(data);
	if (ret) {
		goto probe_err;
	}

	backlight_update_status(data->bl);

	/* Enable Panel */
	if(g_panel_mode == WS_RASPI_MODE) {
		ret = i2c_smbus_write_byte_data(i2c, 0xad, 0x01);
		if (ret)
			dev_err(data->dev, "I2C write failed: %d\n", ret);
	}

	return 0;

probe_err:
	dev_err(data->dev, "failed ret: %d\n", ret);
	return ret;
}

static void ws_backlight_remove(struct i2c_client *i2c)
{
	struct waveshare_bl_data *data = i2c_get_clientdata(i2c);

	data->bl->props.brightness = 0;
	backlight_update_status(data->bl);
}

#ifdef CONFIG_OF
static const struct of_device_id ws_backlight_of_ids[] = {
	{ .compatible = "waveshare,dsi-backlight", },
	{}
};
MODULE_DEVICE_TABLE(of, ws_backlight_of_ids);
#endif

static const struct i2c_device_id ws_backlight_ids[] = {
	{ "waveshare-backlight", 0 },
	{}
};

static struct i2c_driver ws_backlight_driver = {
	.driver = {
		.name = "ws_backlight",
		.of_match_table = ws_backlight_of_ids,
	},
	.probe = ws_backlight_probe,
	.remove = ws_backlight_remove,
	.id_table = ws_backlight_ids,
};
module_i2c_driver(ws_backlight_driver);

MODULE_AUTHOR("eng29 <eng29@luckfox.com>");
MODULE_DESCRIPTION("Waveshare DSI backlight driver");
MODULE_LICENSE("GPL");
