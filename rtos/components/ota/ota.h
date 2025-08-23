/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _SYS_OTA_H_
#define _SYS_OTA_H_

#include "ota_opt.h"

#ifdef __cplusplus
extern "C" {
#endif


#define OTA_TEST                 0

#define IMAGE_SEQ_NUM           (2) /* max number of image sequence */
#define SYS_START_FROM_FW2_FLAG  (0x18CF)
#define SYS_START_FROM_FW1_FLAG  (0x18DF)

#define PART_SECTOR_SIZE        (512)
#define FW_HEAD_SIZE            (512)
#define MINIMUM(x,y)                  ((x)>(y)?(y):(x))

#ifdef RK_OTA
//#define OTA_FW_URL              "http://10.10.10.163:2301/OtaUpdater/android?product=rk2206&version=1.0.1"
//#define OTA_FW_URL              "http://172.16.21.200/update.img"  /*https://172.16.21.157*/
#else
#define OTA_FW_URL              ""
#endif

#ifdef RT_USING_OTA_FROM_LOCAL
#define OTA_FW_LOCAL_PATH       "/sdcard/Firmware.img"
#endif

typedef enum image_state_t_
{
    IMAGE_STATE_NONE       = 0,
    IMAGE_STATE_DOWNLOADED = 1,
    IMAGE_STATE_VERIFIED   = 2,
    IMAGE_STATE_DONE       = 3,
    IMAGE_STATE_UNDEFIND   = 4,
} image_state;

typedef enum ota_status
{
    OTA_STATUS_OK       = 0,
    OTA_STATUS_ERR      = -1,
} ota_status;

typedef enum ota_protocol_t
{
    OTA_PROTOCOL_FILE   = 0,
    OTA_PROTOCOL_HTTP   = 1,
    OTA_PROTOCOL_SPI    = 2,
    OTA_PROTOCOL_MEM    = 3,
} ota_protocol;

typedef enum ota_flash_type_t
{
    OTA_FLASH_SNOR      = 0,
    OTA_FLASH_SNAND     = 1,
    OTA_FLASH_EMMC      = 2,
} ota_flash_type;

typedef struct
{
    rt_uint32_t  os_addr;           /* start addr of rt-thread system area */
    rt_uint32_t  img_max_size;      /* rtthead os image max size*/
    rt_uint8_t   fw_running_slot;   /* running image sequence */
    rt_uint32_t  get_size;

    rt_uint32_t  data_addr;         /* the addr of rt-thread fw data */
    rt_uint32_t  data_max_size;     /* data part max size*/

#ifdef RT_SUPPORT_ROOT_AB
    rt_uint32_t  user_addr;         /* start addr of userdata partition */
    rt_uint32_t  user_max_size;     /* userdata partition max size*/
    rt_uint8_t   user_running_slot; /* running userdata sequence */
    rt_uint32_t  user_get_size;
#endif
} ota_priv_st;


/**
 * OTA image verification algorithm definition
 */
typedef enum ota_verify_t
{
    OTA_VERIFY_NONE     = 0,
#if OTA_OPT_VERIFY_JHASH
    OTA_VERIFY_JHASH    = 1,
#endif
} ota_verify;

/**
 * @brief Image configuration definition
 */
typedef struct image_cfg_t
{
    rt_uint32_t        seq;

    image_state state;
} image_cfg;


#define RK_OTA_DBG_EN 1

#if RK_OTA_DBG_EN
#define RK_OTA_DBG(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf("OTA: " fmt, ## args);       \
    }                                   \
    while(0)
#else
#define RK_OTA_DBG(fmt, args...)  do { } while (0)
#endif

#define RK_OTA_INF(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf("OTA: " fmt, ## args);       \
    }                                   \
    while(0)

#define RK_OTA_ERR(fmt, args...)     \
    do                                  \
    {                                   \
        rt_kprintf("OTA: " fmt, ## args);       \
    }                                   \
    while(0)

/*----------------------------------- Typedefs -------------------------------*/
typedef ota_status(*ota_update_init)(void *url);
typedef ota_status(*ota_update_get)(uint8_t *buf, uint32_t buf_size, uint32_t *recv_size, uint8_t *eof_flag);

extern ota_status ota_init(void);
extern void ota_deinit(void);
extern ota_status ota_update_image(ota_protocol protocol, void *url);
extern ota_status ota_verify_img(ota_verify verify);
extern void ota_reboot(int system_running);

void rk_ota_set_boot_success(void);
int rk_ota_process(ota_protocol prot_type, char *url);
int rk_ota_get_misc_recovery_flag(rt_uint32_t *recovery_flag);
int rk_ota_set_misc_recovery_flag(rt_uint32_t recovery_flag);
rt_bool_t rk_ota_download_loader(unsigned char *data_buf, int size, char *dest_path);
int rk_ota_upgrade_loader_idb(char *name, rt_uint8_t *idb, rt_uint32_t offset, rt_uint32_t size);
int rk_ota_source_init(char *url);
int rk_ota_source_read(rt_uint8_t *buf, rt_uint32_t length);
int rk_ota_source_seek(rt_uint32_t offset, int whence);
int rk_ota_source_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_OTA_H_ */

