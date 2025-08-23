/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff chen   first implementation
 */

#include "rtdef.h"
#include "iomux.h"
#include "hal_base.h"

/**
 * @brief  Config io domian for board of rv1103_evb1
 */

void rt_hw_iodomain_config(void)
{
}

/**
 * @brief  Config iomux for RV1103
 */
void rt_hw_iomux_config(void)
{
    rt_hw_iodomain_config();

#ifdef RT_USING_UART2
    uart2_m0_iomux_config();
#endif

#ifdef RT_USING_BACKLIGHT
    pwm7_ch1_iomux_config();
#endif

#ifdef RT_USING_VOP
    lcdc_rgb3x8_iomux_config();
#endif
}
