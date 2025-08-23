/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_fspi.c
  * @version V1.0
  * @brief   fspi controller interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2023-08-18     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup FSPI
 *  @{
 */

/** @defgroup FSPI_How_To_Use How To Use
 *  @{

    FSPI is a fspi controller layer based on HAL_FSPI. It needs to be combined
 with the corresponding driver layer to complete the transmission of the protocol.

 - deivice Layer(drv_qpipsram/drv_spinand/drv_snor)
 - FSPI controller Layer DRV_FSPI
 - FSPI lower Layer HAL_FSPI
 - SPI Flash devices

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "hal_bsp.h"
#include "hal_base.h"

#include "drv_fspi.h"
#include "drv_clock.h"

#if defined(RT_USING_SNOR_FSPI_HOST) || defined(RT_USING_SPINAND_FSPI_HOST)

/********************* Private MACRO Definition ******************************/
/** @defgroup FSPI_Private_Macro Private Macro
 *  @{
 */

// #define FSPI_DEBUG
#ifdef FSPI_DEBUG
#define fspi_dbg(...)     rt_kprintf(__VA_ARGS__)
#else
#define fspi_dbg(...)
#endif

/** @} */  // FSPI_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup FSPI_Private_Structure Private Structure
 *  @{
 */

/** @} */  // FSPI_Private_Structure

/********************* Private Variable Definition ***************************/
/** @defgroup FSPI_Private_Macro Variable Macro
 *  @{
 */

struct rt_fspi_controller ctrl_buffer[FSPI_CHIP_CNT] =
{
    { .host = &g_fspi0Dev, },
#ifdef FSPI1
    { .host = &g_fspi1Dev, },
#endif
#ifdef FSPI2
    { .host = &g_fspi2Dev, },
#endif
};

/** @} */  // FSPI_Variable_Macro

/********************* Private Function Definition ***************************/
/** @defgroup FSPI_Private_Function Private Function
 *  @{
 */
#ifdef RT_USING_SPINAND_FSPI_CS1
static rt_err_t rt_fspi_cs_gpio_init(struct rt_fspi_cs_gpio *cs_gpio)
{
    HAL_PINCTRL_SetIOMUX(cs_gpio->bank,
                         cs_gpio->pin,
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(cs_gpio->gpio, cs_gpio->pin, GPIO_OUT);
    HAL_GPIO_SetPinLevel(cs_gpio->gpio, cs_gpio->pin, GPIO_HIGH);

    return RT_EOK;
}

static rt_err_t rt_fspi_cs_gpio_take(struct rt_fspi_cs_gpio *cs_gpio)
{
    HAL_GPIO_SetPinLevel(cs_gpio->gpio, cs_gpio->pin, GPIO_LOW);

    return RT_EOK;
}

static rt_err_t rt_fspi_cs_gpio_release(struct rt_fspi_cs_gpio *cs_gpio)
{
    HAL_GPIO_SetPinLevel(cs_gpio->gpio, cs_gpio->pin, GPIO_HIGH);

    return RT_EOK;
}

static rt_err_t rt_fspi_mutex_take(struct rt_fspi_device *fspi_device, rt_int32_t time)
{
    if (fspi_device->ctrl->lock_en)
    {
        return rt_mutex_take(&fspi_device->ctrl->host_lock, time);
    }

    return RT_EOK;
}

static rt_err_t rt_fspi_mutex_release(struct rt_fspi_device *fspi_device)
{
    if (fspi_device->ctrl->lock_en)
    {
        return rt_mutex_release(&fspi_device->ctrl->host_lock);
    }

    return RT_EOK;
}
#else /* #ifdef RT_USING_SPINAND_FSPI_CS1 */
static rt_err_t rt_fspi_mutex_take(struct rt_fspi_device *fspi_device, rt_int32_t time)
{
    return RT_EOK;
}

static rt_err_t rt_fspi_mutex_release(struct rt_fspi_device *fspi_device)
{
    return RT_EOK;
}
#endif

/** @} */  // FSPI_Private_Function

/********************* Public Function Definition ****************************/

rt_err_t rt_fspi_suspend(struct rt_fspi_device *fspi_device)
{
    fspi_dbg("%s enter ... \n", __func__);

    return RT_EOK;
}

rt_err_t rt_fspi_resume(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    ret = HAL_FSPI_Init(host);
    if (host->cell)
    {
        ret = HAL_FSPI_SetDelayLines(host, host->cell);
    }

    fspi_dbg("%s exit ... \n", __func__);

    return ret;
}

bool rt_fspi_is_poll_finished(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    return HAL_FSPI_IsPollFinished(host);
}

rt_err_t rt_fspi_irqhelper(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    return HAL_FSPI_IRQHelper(host);
}

rt_err_t rt_fspi_xfer(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

#ifdef RT_USING_SPINAND_FSPI_CS1
    /* Config that need to be manually adjusted */
    if (fspi_device->ctrl->cur_speed != fspi_device->speed)
    {
        fspi_dbg("%s %d change speed=%d\n", __func__, __LINE__, fspi_device->speed);
        HAL_CRU_ClkSetFreq(host->sclkID, fspi_device->speed);
        fspi_device->ctrl->cur_speed = fspi_device->speed;
    }

    rt_fspi_mutex_take(fspi_device, RT_WAITING_FOREVER);
    if (host->cell != host->xmmcDev[fspi_device->chip_select].cell)
    {
        fspi_dbg("%s %d change cells=%d\n", __func__, __LINE__, host->xmmcDev[fspi_device->chip_select].cell);
        HAL_FSPI_SetDelayLines(host, host->xmmcDev[fspi_device->chip_select].cell);
        host->cell = host->xmmcDev[fspi_device->chip_select].cell;
    }

    /* Configure cs-gpio */
    if (fspi_device->cs_gpio.gpio)
        rt_fspi_cs_gpio_take(&fspi_device->cs_gpio);
#endif

    /* Configure FSPI */
    host->cs = fspi_device->chip_select;
    host->mode = fspi_device->mode;
    ret = HAL_FSPI_SpiXfer(host, op);

#ifdef RT_USING_SPINAND_FSPI_CS1
    if (fspi_device->cs_gpio.gpio)
        rt_fspi_cs_gpio_release(&fspi_device->cs_gpio);
    rt_fspi_mutex_release(fspi_device);
#endif

    if (ret)
    {
        fspi_dbg("%s fail, ret= %d\n", __func__, ret);
    }

    return ret;
}

rt_err_t rt_fspi_xfer_hw_polling(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    /* Configure FSPI */
    host->cs = fspi_device->chip_select;
    host->mode = fspi_device->mode;
    ret = HAL_FSPI_SpiXferHWPolling(host, op);

    if (ret)
    {
        fspi_dbg("%s fail, ret= %d\n", __func__, ret);
    }

    return ret;
}

rt_err_t rt_fspi_xip_config(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op, uint32_t on)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

#ifdef RT_USING_SPINAND_FSPI_CS1
    /* Config that need to be manually adjusted */
    if (fspi_device->ctrl->cur_speed != fspi_device->speed)
    {
        HAL_CRU_ClkSetFreq(host->sclkID, fspi_device->speed);
        fspi_device->ctrl->cur_speed = fspi_device->speed;
    }
    if (host->cell != host->xmmcDev[fspi_device->chip_select].cell)
    {
        HAL_FSPI_SetDelayLines(host, host->xmmcDev[fspi_device->chip_select].cell);
        host->cell = host->xmmcDev[fspi_device->chip_select].cell;
    }
#endif

    /* Configure FSPI */
    host->cs = fspi_device->chip_select;
    host->mode = fspi_device->mode;
    if (op)
    {
        HAL_FSPI_XmmcSetting(host, op);
    }
    ret = HAL_FSPI_XmmcRequest(host, on);

    return ret;
}

uint32_t rt_fspi_get_max_dll_cells(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;
    uint32_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    ret = HAL_FSPI_GetMaxDllCells(host);

    return ret;
}

rt_err_t rt_fspi_set_delay_lines(struct rt_fspi_device *fspi_device, uint16_t cells)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    rt_fspi_mutex_take(fspi_device, RT_WAITING_FOREVER);
    ret = HAL_FSPI_SetDelayLines(host, cells);
    rt_fspi_mutex_release(fspi_device);

    if (!ret)
    {
        host->xmmcDev[fspi_device->chip_select].cell = cells;
        host->cell = cells;
    }

    return ret;
}

rt_err_t rt_fspi_dll_disable(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;
    rt_err_t ret;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    rt_fspi_mutex_take(fspi_device, RT_WAITING_FOREVER);
    ret = HAL_FSPI_DLLDisable(host);
    rt_fspi_mutex_release(fspi_device);

    return ret;
}

int32_t rt_fspi_set_speed(struct rt_fspi_device *fspi_device, uint32_t speed)
{
#ifdef HAL_CRU_MODULE_ENABLED
    struct HAL_FSPI_HOST *host;
#endif
    rt_err_t ret = 0;

    HAL_ASSERT(fspi_device);

    if (fspi_device->ctrl->cur_speed == speed)
        return speed;

#ifdef HAL_CRU_MODULE_ENABLED
    host = fspi_device->ctrl->host;
#if (((FSPI_VER >> 18) & 0x1) == 0x1U) /* Support X8_CAP */
    ret = HAL_CRU_ClkSetFreq(host->sclkID, speed * 2);
#else
    ret = HAL_CRU_ClkSetFreq(host->sclkID, speed);
#endif
#endif
    if (!ret)
    {
        fspi_device->speed = speed;
        fspi_device->ctrl->cur_speed = fspi_device->speed;
        ret = fspi_device->ctrl->cur_speed;
    }

    return ret;
}

rt_err_t rt_fspi_get_speed(struct rt_fspi_device *fspi_device)
{
    return fspi_device->speed;
}

uint32_t rt_fspi_get_xip_mem_data_phys(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    return host->xipMemData;
}

uint32_t rt_fspi_get_xip_mem_code_phys(struct rt_fspi_device *fspi_device)
{
    struct HAL_FSPI_HOST *host;

    HAL_ASSERT(fspi_device);
    host = fspi_device->ctrl->host;

    return host->xipMemCode;
}

rt_err_t rt_fspi_set_mode(struct rt_fspi_device *fspi_device, uint32_t mode)
{
    HAL_ASSERT(fspi_device);
    HAL_ASSERT(!fspi_device->cs_gpio.gpio || (fspi_device->cs_gpio.gpio && !(mode & HAL_SPI_XIP)));

    fspi_device->mode = mode;

    return RT_EOK;
}

int32_t rt_fspi_get_irqnum(struct rt_fspi_device *fspi_device)
{
    HAL_ASSERT(fspi_device);

    return fspi_device->ctrl->host->irqNum;
}

rt_err_t rt_fspi_controller_init(struct rt_fspi_device *fspi_device)
{
    rt_err_t ret;

    HAL_ASSERT(fspi_device);

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(fspi_device->ctrl->host->sclkGate);
    HAL_CRU_ClkEnable(fspi_device->ctrl->host->hclkGate);
#endif

    rt_fspi_mutex_take(fspi_device, RT_WAITING_FOREVER);
    ret = HAL_FSPI_Init(fspi_device->ctrl->host);
    rt_fspi_mutex_release(fspi_device);

    return ret;
}

/** @defgroup FSPI_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  Init FSPI framwork and apply to use.
 * @attention The FSPI will be enabled when board initialization, do not dynamically switch FSPI
 *      unless specifically required.
 */
int rt_hw_fspi_device_register(struct rt_fspi_device *fspi_device)
{
    struct rt_fspi_controller *ctrl;
    int ret = -RT_ERROR;
    char *host_name;

    RT_ASSERT(fspi_device);
    RT_ASSERT(fspi_device->host_id < FSPI_CHIP_CNT);

    /* fspi_controller initial */
    host_name = rt_malloc(RT_NAME_MAX);
    rt_memset(host_name, 0, RT_NAME_MAX);
    rt_sprintf(host_name, "%s%d", "fspi", fspi_device->host_id);
    ctrl = (struct rt_fspi_controller *)rt_device_find(host_name);
    if (!ctrl)
    {
        ctrl = &ctrl_buffer[fspi_device->host_id];

        if (rt_mutex_init(&ctrl->host_lock, host_name, RT_IPC_FLAG_FIFO) != RT_EOK)
        {
            rt_kprintf("Init mutex error\n");
            RT_ASSERT(0);
        }

        ret = rt_device_register(&ctrl->dev, host_name, RT_DEVICE_FLAG_STANDALONE);
        if (ret)
        {
            rt_kprintf("%s fail\n", __func__);
            rt_free(host_name);
            return ret;
        }
        ctrl->initialized = true;
        ctrl->lock_en = false;
    }
    else
    {
        rt_free(host_name);
    }

    /* fspi_device initial */
    if (ctrl->cs_valid & HAL_BIT(fspi_device->chip_select))
    {
        rt_kprintf("%s failed, cs%d already register\n", __func__, fspi_device->chip_select);
        return -RT_EINVAL;
    }

#ifdef RT_USING_XIP
    if (fspi_device->dev_type == DEV_SPINAND && fspi_device->chip_select == 1)
    {
        rt_kprintf("%s failed, when cs1 is valid, close XIP in rtconfig.py\n", __func__);
    }
#endif

#ifdef RT_USING_SPINAND_FSPI_CS1
    if (fspi_device->cs_gpio.gpio)
        rt_fspi_cs_gpio_init(&fspi_device->cs_gpio);
#endif
    ctrl->host->xmmcDev[fspi_device->chip_select].type = fspi_device->dev_type;

    fspi_device->ctrl = ctrl;
    fspi_device->ctrl->cs_valid |= HAL_BIT(fspi_device->chip_select);
    if (fspi_device->chip_select == 1)
        ctrl->lock_en = true;

    return RT_EOK;
}

/** @} */  // FSPI_Public_Function

#endif

/** @} */  // FSPI

/** @} */  // RKBSP_Common_Driver
