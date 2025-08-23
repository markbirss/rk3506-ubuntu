/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_opp.c
  * @version V0.1
  * @brief   otp interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-06-25     Finley.Xiao     first implementation
  *
  ******************************************************************************
  */

#include <rtdef.h>
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_OPP)

#include "hal_base.h"
#include "drv_opp.h"
#include "drv_otp.h"
#include "drv_clock.h"
#include "drv_regulator.h"
#include "drv_thermal.h"

static void opp_set_pvt_config(eCLOCK_Name clk, uint64_t rate, uint32_t len)
{
    struct HAL_PVT_CFG pvt_cfg = {.clk = clk, .rate = rate, .length = len};

    HAL_CRU_PvtConfig(&pvt_cfg);
}

static uint32_t opp_get_volt(struct rk_opp_table *opp_table, uint64_t freq)
{
    uint32_t i = 0;

    for (i = 0; i < opp_table->num_opps; i++)
    {
        if (freq == opp_table->opps[i].rate)
        {
            if (opp_table->is_low_temp && opp_table->low_temp_min_volt &&
                    (opp_table->opps[i].volt < opp_table->low_temp_min_volt))
                return opp_table->low_temp_min_volt;

            return opp_table->opps[i].volt;
        }
    }

    return 0;
}

static rt_err_t opp_set_reg_volt(struct rk_opp_table *opp_table, uint32_t volt)
{
    if (opp_table->set_reg_volt)
        return opp_table->set_reg_volt(opp_table, volt);

#ifdef HAL_PWR_MODULE_ENABLED
    if (opp_table->reg_desc)
        return regulator_set_voltage(opp_table->reg_desc, volt);
#endif

    return RT_EOK;
}

rt_err_t opp_set_rate_nolock(struct rk_opp_table *opp_table, uint64_t target_freq)
{
    uint64_t cur_freq = clk_get_rate(opp_table->clk_id);
    uint32_t target_volt = opp_get_volt(opp_table, target_freq);
    rt_err_t error = RT_EOK;

    if (!opp_table || !target_freq || !target_volt)
        return RT_EOK;

    if (target_freq >= cur_freq)
    {
        error = opp_set_reg_volt(opp_table, target_volt);
        if (error != RT_EOK)
        {
            rt_kprintf("failed to set volt %u\n", target_volt);
            return error;
        }
    }

    error = clk_set_rate(opp_table->clk_id, target_freq);
    if (error != RT_EOK)
    {
        rt_kprintf("failed to set freq %u\n", (uint32_t)target_freq);
        return error;
    }

    if (target_freq < cur_freq)
    {
        error = opp_set_reg_volt(opp_table, target_volt);
        if (error != RT_EOK)
        {
            rt_kprintf("failed to set volt %u\n", target_volt);
            return error;
        }
    }

    opp_table->cur_freq = target_freq;
    opp_table->cur_volt = target_volt;

    return RT_EOK;
}

rt_err_t opp_set_rate(struct rk_opp_table *opp_table, uint64_t target_freq)
{
    rt_err_t error = RT_EOK;

    if (!opp_table)
        return RT_EOK;

    rt_mutex_take(&opp_table->lock, RT_WAITING_FOREVER);
    error = opp_set_rate_nolock(opp_table, target_freq);
    rt_mutex_release(&opp_table->lock);
    if (error != RT_EOK)
    {
        rt_kprintf("opp failed to set rate\n");
    }

    return error;
}

static void opp_adjust_pvt_config(struct rk_opp_table *opp_table)
{
    int delta_sel = opp_table->pvtm_sel;
    int delta_len = 0;
    uint32_t i = 0;

    if (!opp_table->clk_id)
        return;
    if (opp_table->pvtm_sel == UINT_MAX)
        return;
    if (!opp_table->pvtm_cfg)
        return;

    for (i = 0; i < opp_table->num_opps; i++)
    {
        if (!opp_table->opps[i].len)
            continue;

        if (opp_table->pvtm_cfg->volt_step)
        {
            if (opp_table->volt_delta_len)
            {
                opp_table->opps[i].len += opp_table->volt_delta_len;
            }
            else if (opp_table->pvtm_sel <= opp_table->typical_sel)
            {
                delta_sel -= opp_table->typical_sel;
                delta_len = delta_sel * opp_table->pvtm_cfg->len_step;
                opp_table->opps[i].len += delta_len;
            }
            opp_set_pvt_config(opp_table->clk_id, opp_table->opps[i].rate,
                               opp_table->opps[i].len);
        }
        else if (opp_table->pvtm_cfg->len_step)
        {
            delta_len = opp_table->pvtm_sel * opp_table->pvtm_cfg->len_step;
            opp_table->opps[i].len += delta_len;
            opp_set_pvt_config(opp_table->clk_id, opp_table->opps[i].rate,
                               opp_table->opps[i].len);
        }
    }
}

static void opp_adjust_volt(struct rk_opp_table *opp_table)
{
    uint32_t target_volt = 0, delta_sel = 0, delta_volt = 0;
    uint32_t i = 0;

    if (opp_table->pvtm_sel == UINT_MAX)
        return;
    if (!opp_table->pvtm_cfg || !opp_table->pvtm_cfg->volt_step)
        return;

    for (i = 0; i < opp_table->num_opps; i++)
    {
        if (!opp_table->opps[i].volt)
            continue;
        if (opp_table->pvtm_sel <= opp_table->typical_sel)
            continue;
        delta_sel = opp_table->pvtm_sel - opp_table->typical_sel;
        delta_volt = delta_sel * opp_table->pvtm_cfg->volt_step;
        target_volt = opp_table->opps[i].volt - delta_volt;
        if (target_volt < opp_table->opps[i].volt_min)
        {
            opp_table->opps[i].volt = opp_table->opps[i].volt_min;
            delta_volt = opp_table->opps[i].volt_min - target_volt;
            opp_table->volt_delta_len = delta_volt / opp_table->pvtm_cfg->volt_step;
        }
        else
        {
            opp_table->opps[i].volt = target_volt;
        }
    }
}

rt_err_t opp_init_pvt_config(struct rk_opp_table *opp_table)
{
    uint8_t otp_length = 0;
    uint32_t i;

    if (!opp_table)
        return RT_EOK;

#if defined(RT_USING_OTP)
    if (opp_table->otp && opp_table->otp->pvt_len_offset)
    {
        if (otp_read(opp_table->otp->pvt_len_offset, 1, &otp_length) != RT_EOK)
        {
            rt_kprintf("failed to read otp length\n");
        }
        else
        {
            rt_kprintf("%s otp length=%u\n", opp_table->name, otp_length);
            if (otp_length > 0)
                otp_length -= 1;
        }
    }
#endif

    for (i = 0; i < opp_table->num_opps; i++)
    {
        if (!opp_table->opps[i].len)
            continue;
        opp_table->opps[i].len += otp_length;
        opp_set_pvt_config(opp_table->clk_id, opp_table->opps[i].rate,
                           opp_table->opps[i].len);
    }

    return RT_EOK;
}

rt_err_t opp_get_supply(struct rk_opp_table *opp_table)
{
    if (!opp_table || !opp_table->pwr_id)
        return RT_EOK;

#ifdef HAL_PWR_MODULE_ENABLED
    opp_table->reg_desc = regulator_get_desc_by_pwrid(opp_table->pwr_id);
    if (!opp_table->reg_desc)
    {
        return RT_ERROR;
    }
#endif

    return RT_EOK;
}

static rt_err_t opp_get_pvtm_sel(struct rk_opp_table *opp_table)
{
    const struct rk_pvtm_config *pvtm_cfg = opp_table->pvtm_cfg;
    const struct rk_pvtm_table *pvtm_tbl = opp_table->pvtm_tbl;
    uint64_t cur_freq = clk_get_rate(opp_table->clk_id);
    rt_err_t error = RT_EOK;
    int value = 0, temp = 0, diff_temp = 0, diff_value = 0, prop_temp = 0;
    uint32_t i = 0;

    if (!pvtm_cfg || !pvtm_tbl)
        return RT_EOK;

    if (pvtm_cfg->freq >= cur_freq)
    {
        error = opp_set_reg_volt(opp_table, pvtm_cfg->volt);
        if (error != RT_EOK)
        {
            rt_kprintf("failed to set volt %u\n", pvtm_cfg->volt);
            return error;
        }
    }

    error = clk_set_rate(opp_table->clk_id, pvtm_cfg->freq);
    if (error != RT_EOK)
    {
        rt_kprintf("failed to set freq %u\n", (uint32_t)pvtm_cfg->freq);
        return error;
    }

    if (pvtm_cfg->freq < cur_freq)
    {
        error = opp_set_reg_volt(opp_table, pvtm_cfg->volt);
        if (error != RT_EOK)
        {
            rt_kprintf("failed to set volt %u\n", pvtm_cfg->volt);
            return error;
        }
    }

    HAL_DelayUs(1200);

    value = READ_REG(*(uint32_t *)(pvtm_cfg->base + pvtm_cfg->offset));

    temp = tsadc_get_temp(pvtm_cfg->tsadc_ch);
    diff_temp = temp - pvtm_cfg->ref_temp;
    if (diff_temp < 0)
    {
        prop_temp = pvtm_cfg->temp_prop[0];
    }
    else
    {
        prop_temp = pvtm_cfg->temp_prop[1];
    }
    diff_value = diff_temp * prop_temp / 1000;
    value += diff_value;

    for (i = 0; pvtm_tbl[i].value != UINT_MAX; i++)
    {
        if (value >= pvtm_tbl[i].value)
        {
            opp_table->pvtm_sel = pvtm_tbl[i].sel;
        }
    }
    rt_kprintf("%s temp=%d pvtm=%u %d sel=%u\n", opp_table->name, temp, value,
               diff_value, opp_table->pvtm_sel);

    return RT_EOK;
}

static void opp_adjust_opp_table(struct rk_opp_table *opp_table)
{
    opp_adjust_volt(opp_table);
    opp_adjust_pvt_config(opp_table);
}

static rt_err_t opp_init_freq(struct rk_opp_table *opp_table)
{
    rt_err_t error = RT_EOK;

    opp_table->is_low_temp = true;

    if (opp_table->init_freq)
    {
        error = opp_set_rate(opp_table, opp_table->init_freq);
    }
    else
    {
        opp_table->cur_freq = clk_get_rate(opp_table->clk_id);
    }

    return error;
}

static void opp_low_temp_adjust(struct rk_opp_table *opp_table, int temp)
{
    bool is_low_temp = false;

    if (!opp_table->low_temp_min_volt)
        return;

    if (temp <= opp_table->low_temp)
    {
        is_low_temp = true;
    }
    else if (temp > (opp_table->low_temp + 5))
    {
        is_low_temp = false;
    }

    if (opp_table->is_low_temp != is_low_temp)
    {
        opp_table->is_low_temp = is_low_temp;
        if (opp_set_rate(opp_table, opp_table->cur_freq) != RT_EOK)
        {
            rt_kprintf("%s failed to set rate\n", __func__);
        }
    }
}

void opp_adjust_by_temp(int temp)
{
    struct rk_opp_table *opp_table = g_opp_table;

    if (!opp_table)
        return;

    while (opp_table->name)
    {
        opp_low_temp_adjust(opp_table, temp);
        opp_table++;
    }
}

struct rk_opp_table *opp_get_opp_table(eCLOCK_Name clk_id)
{
    struct rk_opp_table *opp_table = g_opp_table;

    if (!opp_table)
        return NULL;

    while (opp_table->name)
    {
        if (opp_table->clk_id == clk_id)
            return opp_table;
        opp_table++;
    }

    return NULL;
}

int opp_init(void)
{
    struct rk_opp_table *opp_table = g_opp_table;
    rt_err_t error = RT_EOK;

    if (!opp_table)
        return RT_EOK;

    while (opp_table->name)
    {
        if (rt_mutex_init(&opp_table->lock, opp_table->name, RT_IPC_FLAG_FIFO) != RT_EOK)
        {
            rt_kprintf("opp failed to init mutex\n");
            RT_ASSERT(0);
        }

        error = opp_init_pvt_config(opp_table);
        if (error != RT_EOK)
        {
            rt_kprintf("opp failed to init pvt config\n");
            return error;
        }

        error = opp_get_supply(opp_table);
        if (error != RT_EOK)
        {
            rt_kprintf("opp failed to get supply\n");
            return error;
        }

        error = opp_get_pvtm_sel(opp_table);
        if (error != RT_EOK)
        {
            rt_kprintf("opp failed to get pvtm sel\n");
            return error;
        }

        opp_adjust_opp_table(opp_table);

        error = opp_init_freq(opp_table);
        if (error != RT_EOK)
        {
            rt_kprintf("opp failed to init freq\n");
            return error;
        }

        opp_table++;
    }

    return RT_EOK;
}
INIT_DEVICE_EXPORT(opp_init);

void opp_dump(int argc, char **argv)
{
    struct rk_opp_table *opp_table = g_opp_table;
    uint32_t volt = 0;
    uint32_t i = 0;

    if (!opp_table)
        return;

    while (opp_table->name)
    {
        rt_mutex_take(&opp_table->lock, RT_WAITING_FOREVER);
        rt_kprintf("%s:\n", opp_table->name);
        for (i = 0; i < opp_table->num_opps; i++)
        {
            if (opp_table->is_low_temp && opp_table->low_temp_min_volt &&
                    (opp_table->opps[i].volt < opp_table->low_temp_min_volt))
                volt = opp_table->low_temp_min_volt;
            else
                volt = opp_table->opps[i].volt;
            rt_kprintf("rate=%u volt=%u len=%u\n",
                       (uint32_t)opp_table->opps[i].rate, volt, opp_table->opps[i].len);
        }
        rt_mutex_release(&opp_table->lock);
        opp_table++;
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(opp_dump, opp driver dump. e.g: opp_dump);
#endif

#endif
