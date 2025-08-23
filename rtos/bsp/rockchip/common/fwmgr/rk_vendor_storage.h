/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __PLAT_RK_VENDOR_STORAGE_H
#define __PLAT_RK_VENDOR_STORAGE_H

#ifndef u32
typedef unsigned int    u32;
#endif
#ifndef u16
typedef unsigned short  u16;
#endif
#ifndef u8
typedef unsigned char   u8;
#endif

#define RSV_ID              0
#define SN_ID               1
#define WIFI_MAC_ID         2
#define LAN_MAC_ID          3
#define BT_MAC_ID           4

#define VENDOR_HEAD_TAG         0x524B5644
#define FLASH_VENDOR_PART_SIZE      8
#define VENDOR_PART_SIZE        8

#define VENDOR_ALIGN(x, mask)        (((x) + (mask) - 1) & ~((mask) - 1))

#define RK_VENDOR_INF(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf("VENDOR: " fmt, ## args);       \
    }                                   \
    while(0)

struct vendor_item
{
    u16  id;
    u16  offset;
    u16  size;
    u16  flag;
};

struct vendor_info
{
    u32 tag;
    u32 version;
    u16 next_index;
    u16 item_num;
    u16 free_offset;
    u16 free_size;
    struct  vendor_item item[62]; /* 62 * 8 */
    u8  data[VENDOR_PART_SIZE * 512 - 512 - 8];
    u32 hash;
    u32 version2;
};

struct flash_vendor_info
{
    u32 tag;
    u32 version;
    u16 next_index;
    u16 item_num;
    u16 free_offset;
    u16 free_size;
    struct  vendor_item item[62]; /* 62 * 8 */
    u8  data[FLASH_VENDOR_PART_SIZE * 512 - 512 - 8];
    u32 hash;
    u32 version2;
};

int rk_vendor_read(u32 id, void *pbuf, u32 size);
int rk_vendor_write(u32 id, void *pbuf, u32 size);
int rk_vendor_register(void *read, void *write);
rt_bool_t is_rk_vendor_ready(void);

#endif
