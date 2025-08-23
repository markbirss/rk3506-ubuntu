/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_FSPI.h
  * @version V1.0
  * @brief   fspi driver headfile
  *
  * Change Logs:
  * Date           Author          Notes
  * 2023-08-18     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#if defined(RT_USING_SNOR_FSPI_HOST) || defined(RT_USING_SPINAND_FSPI_HOST)

#ifndef __DRV_FSPI_H__
#define __DRV_FSPI_H__

#define RT_FSPI_SPEED_THRESHOLD HAL_FSPI_SPEED_THRESHOLD

struct rt_fspi_cs_gpio
{
    struct GPIO_REG *gpio;
    eGPIO_bankId bank;
#ifdef HAL_GPIO_MODULE_ENABLED
    ePINCTRL_GPIO_PINS pin;
#endif
};

struct rt_fspi_controller
{
    struct rt_device dev;
    struct HAL_FSPI_HOST *host;
    struct rt_mutex host_lock;
    uint32_t cur_speed;
    uint32_t cs_valid;
    bool initialized;
    bool lock_en;
};

struct rt_fspi_device
{
    struct rt_fspi_controller *ctrl;
    uint8_t host_id;
    eFSPI_devType dev_type;
    uint8_t chip_select;
    struct rt_fspi_cs_gpio cs_gpio;
    uint32_t speed;
    uint32_t mode;
    uint32_t cell;
};

extern struct rt_fspi_device g_fspi_spinor;
extern struct rt_fspi_device g_fspi_spinand;

rt_err_t rt_fspi_suspend(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_resume(struct rt_fspi_device *fspi_device);
bool rt_fspi_is_poll_finished(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_irqhelper(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_xfer(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op);
rt_err_t rt_fspi_xfer_hw_polling(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op);
rt_err_t rt_fspi_xip_config(struct rt_fspi_device *fspi_device, struct HAL_SPI_MEM_OP *op, uint32_t on);
uint32_t rt_fspi_get_max_dll_cells(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_set_delay_lines(struct rt_fspi_device *fspi_device, uint16_t cells);
rt_err_t rt_fspi_dll_disable(struct rt_fspi_device *fspi_device);
int32_t rt_fspi_set_speed(struct rt_fspi_device *fspi_device, uint32_t speed);
rt_err_t rt_fspi_get_speed(struct rt_fspi_device *fspi_device);
uint32_t rt_fspi_get_xip_mem_data_phys(struct rt_fspi_device *fspi_device);
uint32_t rt_fspi_get_xip_mem_code_phys(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_set_mode(struct rt_fspi_device *fspi_device, uint32_t mode);
int32_t rt_fspi_get_irqnum(struct rt_fspi_device *fspi_device);
rt_err_t rt_fspi_controller_init(struct rt_fspi_device *fspi_device);
int rt_hw_fspi_device_register(struct rt_fspi_device *fspi_device);

#endif

#endif
