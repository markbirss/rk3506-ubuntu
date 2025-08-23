/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    mini_ftl.h
  * @version V1.0
  * @brief   spi nand special mini ftl head file
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-07-23     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#ifndef MINI_FTL_H__
#define MINI_FTL_H__

rt_err_t mini_ftl_map_table_init(rt_mtd_nand_t mtd, rt_uint32_t start, rt_uint32_t size, char *mblk_name);
rt_err_t mini_ftl_read(rt_mtd_nand_t mtd, rt_uint8_t *data_buf, rt_uint32_t from, rt_uint32_t length);
rt_err_t mini_ftl_write(rt_mtd_nand_t mtd, const rt_uint8_t *data_buf, rt_uint32_t to, rt_uint32_t length);
rt_err_t mini_ftl_erase(rt_mtd_nand_t mtd, rt_uint32_t addr, size_t len);

#endif