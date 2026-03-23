/*
 *  qiyang_reset_power.c 
 *
 *  Author         	jiang
 *  Email           
 *  Create time     2024-8-30
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/ioctl.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/of_gpio.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/device.h>


//#define QY_DEBUG

struct QYRP {
    long qiyang_power_io;
    long qiyang_reset_io;
    char qiyang_power_str[80];
    char qiyang_reset_str[80];
	struct gpio_desc *qy_rest;
    struct gpio_desc *qy_rest_4g;
    struct gpio_desc *qy_wifi_tf;
};

enum QYRP_ENUM {
    RESET,
    POWER
};

/*---------------------------------------------
RK3506只有一个mmc控制器
uboot传参 wifi_tf=1 打开wifi wifi_tf=0 打开tf
---------------------------------------------*/
int wifi_tf_swich = 0;

int wifi_tf_swich_check(void)
{
	return wifi_tf_swich;
}

static int __init wifi_tf_swich_get(char *str)
{
	if (!str)
		return 0;
	if(!strcmp("1",str)) {
		wifi_tf_swich = 1;
	} 
	return 1;
}
__setup("wifi_tf=", wifi_tf_swich_get);

static int reset_io(struct platform_device *pdev)
{
    int ret;
    int msec = 1;
    struct device_node *np = pdev->dev.of_node;
    struct QYRP *priv = platform_get_drvdata(pdev);
	
    if (!np)
        return 0;
    ret = of_property_read_u32(np, "reset-duration", &msec);
    /* A sane reset duration should not be longer than 1s */
    if (!ret && msec > 1000)
	{msec = 1;}
	priv->qy_rest=devm_gpiod_get_optional(&pdev->dev, "reset", 0);
	if(!IS_ERR(priv->qy_rest))
	{
		//printk("qy_yang_erro_%s\n",pdev->dev.of_node->name);
		//return -ENODEV;
        gpiod_direction_output(priv->qy_rest, 1);
        gpiod_set_value(priv->qy_rest, 1);
	}

    priv->qy_rest_4g=devm_gpiod_get_optional(&pdev->dev, "reset-4g", 0);
	if(!IS_ERR(priv->qy_rest_4g))
	{
		//printk("qy_yang_erro_%s\n",pdev->dev.of_node->name);
		//return -ENODEV;
        gpiod_direction_output(priv->qy_rest_4g, 1);
        gpiod_set_value(priv->qy_rest_4g, 1);
    }

    priv->qy_wifi_tf=devm_gpiod_get_optional(&pdev->dev, "wifi-tf", 0);
	if(!IS_ERR(priv->qy_wifi_tf))
	{
		//printk("qy_yang_erro_%s\n",pdev->dev.of_node->name);
		//return -ENODEV;
        gpiod_direction_output(priv->qy_wifi_tf, 1);
        gpiod_set_value(priv->qy_wifi_tf, 1);
    }

	msleep(msec);
    if(!IS_ERR(priv->qy_rest))
	    gpiod_set_value(priv->qy_rest, 0);

    if(!IS_ERR(priv->qy_rest_4g))    
        gpiod_set_value(priv->qy_rest_4g, 0);

    if(!IS_ERR(priv->qy_wifi_tf))
    {
        if(wifi_tf_swich==1)   //切换WIFI
        {
            gpiod_set_value(priv->qy_wifi_tf, 1);
        }else if(wifi_tf_swich==0)  //切换TF
        {
            gpiod_set_value(priv->qy_wifi_tf, 0);
        }
        gpiod_export(priv->qy_wifi_tf,0);
    }  
    return 0;
}

static int rp_probe(struct platform_device *pdev)
{
    int ret;
    //struct device_node *np = pdev->dev.of_node;
    struct device *dev = &pdev->dev;
    struct QYRP *qy_rp;
    #ifdef QY_DEBUG
        printk("qy___________________>node %s probed \r\n", np->name);
    #endif
	
    qy_rp = devm_kzalloc(dev, sizeof(struct QYRP), GFP_KERNEL);
    platform_set_drvdata(pdev, qy_rp);
    ret = reset_io(pdev);
    if (ret < 0)
        return ret;

    return 0;
}

static int rp_remove(struct platform_device *pdev)
{
    // struct QYRP *priv = platform_get_drvdata(pdev);
    // devm_gpio_free(&pdev->dev, priv->qiyang_reset_io);
    // devm_gpio_free(&pdev->dev, priv->qiyang_power_io);
    // devm_kfree(&pdev->dev, priv);
    return 0;
}

static const struct of_device_id qiyang_rp_dt_match[] = {
    { .compatible = "qiyang,qiyang_eth" },
    { .compatible = "qiyang,qiyang_usb" },
    { .compatible = "qiyang,qiyang_wk2124" },
    { .compatible = "qiyang,qiyang_dm9621a" },
    { .compatible = "qiyang,qiyang_buletooth" },
    { .compatible = "qiyang,qiyang_4G" },
    { .compatible = "qiyang,qiyang_gprs" },
    { .compatible = "qiyang,qiyang_lvds" },
    { .compatible = "qiyang,qiyang_pcie" },

    { }
};
MODULE_DEVICE_TABLE(of, qiyang_rp_dt_match);

static struct platform_driver qiyang_reset_power_driver = {
    .driver ={
        .name = "imx_qiyang_rq",
        .of_match_table = of_match_ptr(qiyang_rp_dt_match),
    },
    .probe = rp_probe,
    .remove = rp_remove,
};

module_platform_driver(qiyang_reset_power_driver);

MODULE_AUTHOR("jiang");
MODULE_DESCRIPTION("IMX reset and power");
MODULE_LICENSE("GPL v2");