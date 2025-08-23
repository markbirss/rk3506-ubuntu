/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board_wlan.c
  * @author  aaron sun
  * @version V0.1
  * @date    25-JUN-2019
  * @brief   wice for rtt wlan struct interface
  *
  ******************************************************************************
  */
#include "board_wifibt.h"

#ifdef RT_USING_WIFI
#include <wlan_mgnt.h>
#include <wlan_cfg.h>
#include <wlan_prot.h>

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

static int rk_wifibt_32k_enable(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1, BT_32K_CLK_OUT, PIN_CONFIG_MUX_FUNC3);
    HAL_CRU_ClkEnable(CLK_BT32K_GATE);
    HAL_CRU_ClkSetFreq(CLK_32K, 32768);

    return 1;
}
#ifdef RT_CYPRESS_WIFI_SDIO
INIT_PREV_EXPORT(rk_wifibt_32k_enable);
#endif

RT_WEAK int rk_wifi_host_reg_on(void)
{
    rt_kprintf("\n %s \n", __func__);

    /* register power on */
    HAL_PINCTRL_SetIOMUX(WIFI_POWER_ON_GPIO_BANK, WIFI_POWER_ON_GPIO_PIN, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(WIFI_POWER_ON_GPIO_GRP, WIFI_POWER_ON_GPIO_PIN, GPIO_OUT);
    HAL_GPIO_SetPinLevel(WIFI_POWER_ON_GPIO_GRP, WIFI_POWER_ON_GPIO_PIN, GPIO_LOW);
    rt_thread_delay(300);
    HAL_GPIO_SetPinLevel(WIFI_POWER_ON_GPIO_GRP, WIFI_POWER_ON_GPIO_PIN, GPIO_HIGH);
    rt_thread_delay(300);

    return 0;
}

RT_WEAK RT_UNUSED void rk_wifi_host_wake_register(void (*call_back)(void *))
{
    rt_err_t ret;
    rt_kprintf("\n %s \n", __func__);

    HAL_PINCTRL_SetIOMUX(WIFI_WAKEUP_GPIO_BANK, WIFI_WAKEUP_GPIO_PIN, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(WIFI_WAKEUP_GPIO_GRP, WIFI_WAKEUP_GPIO_PIN, GPIO_IN);
    ret = rt_pin_attach_irq(BANK_PIN(WIFI_WAKEUP_GPIO_BANK, WIFI_WAKEUP_GPIO_PIN_NUM), PIN_IRQ_MODE_RISING, call_back, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_pin_irq_enable(BANK_PIN(WIFI_WAKEUP_GPIO_BANK, WIFI_WAKEUP_GPIO_PIN_NUM), PIN_IRQ_DISABLE);
    RT_ASSERT(ret == RT_EOK);

}

RT_WEAK RT_UNUSED void rk_wifi_host_wake_enable()
{
    rt_err_t ret;
    rt_kprintf("\n %s \n", __func__);

    ret = rt_pin_irq_enable(BANK_PIN(WIFI_WAKEUP_GPIO_BANK, WIFI_WAKEUP_GPIO_PIN_NUM), PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);
}

RT_WEAK RT_UNUSED void rk_wifi_host_wake_disable()
{
    rt_err_t ret;
    rt_kprintf("\n %s \n", __func__);

    ret = rt_pin_irq_enable(BANK_PIN(WIFI_WAKEUP_GPIO_BANK, WIFI_WAKEUP_GPIO_PIN_NUM), PIN_IRQ_DISABLE);
    RT_ASSERT(ret == RT_EOK);
}

uint32_t rk_bt_rst(void)
{
    rt_thread_mdelay(50);
    HAL_GPIO_SetPinLevel(BT_GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_LOW);
    rt_thread_mdelay(50);
    HAL_GPIO_SetPinLevel(BT_GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_HIGH);
    rt_thread_mdelay(100);
    rt_kprintf("BT POWER On \n");

    return 0;
}

uint32_t rk_bt_power_down(void)
{
    HAL_GPIO_SetPinLevel(BT_GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_LOW);
    rt_thread_delay(100);
    HAL_GPIO_SetPinDirection(BT_GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_IN);

    HAL_PINCTRL_SetIOMUX(BT_GPIO_PORT, BT_32K_CLK_OUT, PIN_CONFIG_MUX_FUNC0);
    HAL_CRU_ClkDisable(CLK_BT32K_GATE);

    return 1;
}

void rk_bt_init_gpio(void)
{
    // setup bt_reg_on
    HAL_PINCTRL_SetIOMUX(BT_GPIO_PORT, BT_POWER_GPIO_PIN, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(BT_GPIO_PORT_BASE, BT_POWER_GPIO_PIN, GPIO_OUT);

    // setup uart
    HAL_PINCTRL_SetIOMUX(BT_GPIO_PORT,
                         BT_HOST_RX_PIN |
                         BT_HOST_TX_PIN |
                         BT_HOST_CTS_PIN |
                         BT_HOST_RTS_PIN,
                         BT_PIN_CONFIG_MUX_FUNC_NUM);
}

extern struct rk_mmc_platform_data rk_mmc_table[];
static void rk_wifi_sta(int argc, char **argv)
{
    rk_wifi_host_reg_on();

#ifdef RT_CYPRESS_WIFI_SDIO
    mmcsd_change(rk_mmc_table[0].drv->host);
#endif

    rt_wlan_set_mode("wlan0", RT_WLAN_STATION);
}
MSH_CMD_EXPORT(rk_wifi_sta, wifi sta mode);

static void rk_wifi_ap(int argc, char **argv)
{
    rt_wlan_set_mode("wlan1", RT_WLAN_AP);
}
MSH_CMD_EXPORT(rk_wifi_ap, wifi ap mode);
#endif
